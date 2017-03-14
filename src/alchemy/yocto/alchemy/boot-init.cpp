#include "yocto/alchemy/boot.hpp"
#include "yocto/math/core/lu.hpp"
#include "yocto/math/core/svd.hpp"
#include "yocto/math/core/tao.hpp"

#include "yocto/math/opt/bracket.hpp"
#include "yocto/math/opt/minimize.hpp"
#include "yocto/math/opt/cgrad.hpp"

#include "yocto/math/core/adjoint.hpp"


namespace yocto
{

    using namespace math;

    namespace alchemy
    {

        namespace
        {
            static const char fn[] = "boot.initialize: ";

            class BootInit
            {
            public:
                equilibria    &eqs;
                const library &lib;
                const size_t   M;
                const size_t   N;
                const size_t   Nc;
                vector<double> Cstar;
                vector<double> U;
                vector<double> Lam;
                vector<double> V;
                vector<bool>   used;
                vector<double> Ctry;
                vector<double> beta;
                vector<double> C;
                matrix<double> P;
                matrix<double> Q;

                numeric<double>::scalar_field E;
                numeric<double>::vector_field G;

                inline BootInit(equilibria                             &__eqs,
                                const array<boot::constraint::pointer> &ld,
                                const double                            t ) :
                eqs( __eqs ),
                lib( *eqs.pLib  ),
                M(lib.size()),
                N(eqs.size()),
                Nc(ld.size()),
                Cstar(M),
                U(Nc),
                Lam(Nc),
                V(N),
                used(N,true),
                Ctry(M),
                beta(M),
                C(M),
                P(),
                Q(),
                E(this, & BootInit::call_E ),
                G(this, & BootInit::call_G )
                {
                    if(Nc+N!=M) throw exception("%s: #constraints=%u + #equilibria=%u != #species=%u", fn, unsigned(Nc), unsigned(N), unsigned(M) );

                    // special cases

                    P.make(Nc,M);

                    for(size_t i=1;i<=Nc;++i)
                    {
                        const boot::constraint &cc = *ld[i];
                        Lam[i] = cc.value;
                        for( boot::constraint::const_iterator it = cc.begin(); it != cc.end(); ++it)
                        {
                            const boot::component &comp = *it;
                            const size_t j = comp.sp->indx;
                            P[i][j] = comp.weight;
                        }
                    }
                    std::cerr << "P  =" << P << std::endl;
                    std::cerr << "Lam=" << Lam << std::endl;

                    // special cases


                    // generic case, build Cstar
                    {
                        matrix<double> PP(Nc);
                        tao::mmul_rtrn(PP, P, P);
                        std::cerr << "PP=" << PP << std::endl;
                        if(!LU<double>::build(PP))
                        {
                            throw exception("%s: singular constraints set", fn);
                        }
                        tao::set(U,Lam);
                        LU<double>::solve(PP,U);
                        std::cerr << "Ustar=" << U << std::endl;
                    }
                    tao::mul_trn(Cstar, P, U);
                    std::cerr << "Cstar=" << Cstar << std::endl;
                    Q.make(N,M);
                    if(!svd<double>::orthonormal(Q,P))
                    {
                        throw exception("%s: unexpected singular constraints set, no orthonormal set",fn);
                    }
                    std::cerr << "Q0=" << Q << std::endl;
                    svd<double>::truncate(Q);
                    std::cerr << "Q=" << Q << std::endl;

                    std::cerr << "// moving V" << std::endl;
                    moveV(C,V);
                    std::cerr << "V=" << V << std::endl;
                    std::cerr << "C=" << C << std::endl;

                    // initialize Gamma, Phi, K
                    eqs.computePhi(C,t);


                }


                double call_E(const array<double> &Vtry)
                {
                    gen_C(Ctry,Vtry);
                    double ans = 0;
                    for(size_t i=M;i>0;--i)
                    {
                        if(eqs.active[i])
                        {
                            const double Ci = Ctry[i];
                            if(Ci<=0) ans += Ci*Ci;
                        }
                    }
                    return ans*0.5;
                }

                void call_G(array<double> &Grad, const array<double> &Vtry)
                {
                    gen_C(Ctry,Vtry);
                    for(size_t i=M;i>0;--i)
                    {
                        beta[i] = 0;
                        if(eqs.active[i])
                        {
                            const double Ci = Ctry[i];
                            if(Ci<0)
                            {
                                beta[i] = Ci;
                            }
                        }
                    }
                    tao::mul(Grad,Q,beta);
                }

                inline void gen_C(array<double> &Cout, const array<double> &Vtry) throw()
                {
                    tao::set(Cout,Cstar);
                    tao::mul_add_trn(Cout,Q,V);
                }

                // move V to get the most valid composition
                inline void moveV(array<double> &Ctry, array<double> &Vtry)
                {
                    (void)cgrad<double>::optimize(E,
                                                  G,
                                                  Vtry,
                                                  used,
                                                  0.0);
                    gen_C(Ctry,Vtry);
                    eqs.validate(Ctry);
                }



            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(BootInit);
            };

        }


        void boot:: initialize( array<double> &C0, equilibria &eqs, const double t)
        {
            BootInit ini(eqs,constraints,t);
        }



    }

}

