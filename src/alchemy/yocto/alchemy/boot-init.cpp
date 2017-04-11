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
                vector<double> dV;
                vector<double> Vtmp;
                vector<bool>   used;
                vector<double> Ctmp;
                vector<double> beta;
                vector<double> C;
                matrix<double> P;
                matrix<double> Q;

                numeric<double>::scalar_field E;
                numeric<double>::vector_field G;
                numeric<double>::function     F;

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
                dV(N),
                Vtmp(N),
                used(N,true),
                Ctmp(M),
                beta(M),
                C(M),
                P(),
                Q(),
                E(this, & BootInit::call_E ),
                G(this, & BootInit::call_G ),
                F(this, & BootInit::call_F )
                {
                    if(Nc+N!=M)
                        throw exception("%s: #constraints=%u + #equilibria=%u != #species=%u", fn, unsigned(Nc), unsigned(N), unsigned(M) );

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

                    //__________________________________________________________
                    //
                    // special cases
                    //__________________________________________________________


                    //__________________________________________________________
                    //
                    // generic case, build Cstar
                    //__________________________________________________________
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

                    //__________________________________________________________
                    //
                    // initialize C and V
                    //__________________________________________________________
                    eqs.computeK(t);
                    eqs.warm_up(C);
                    std::cerr << "Cwarm=" << C << std::endl;
                    tao::mul(V,Q,C);
                    gen_C(C,V);
                    std::cerr << "C0=" << C << std::endl;
                    std::cerr << "V0=" << V << std::endl;

                    std::cerr << "// moving V" << std::endl;
                    moveV(C,V);
                    std::cerr << "V1=" << V << std::endl;
                    std::cerr << "C1=" << C << std::endl;
                    std::cerr << "Nu =" << eqs.Nu    << std::endl;

                    matrix<double> &PhiQ  = eqs.Chi;
                    matrix<double> &Phi   = eqs.Phi;
                    array<double>  &Gamma = eqs.Gamma;

                    //__________________________________________________________
                    //
                    // update Gamma, Phi
                    //__________________________________________________________

                    eqs.updatePhi(C);
                    static const double shrink = 1.0 / numeric<double>::gold;
                    double gam0 = F(0);

                LOOP:
                    std::cerr << "Gam=" << Gamma << std::endl;
                    std::cerr << "gam0=" << gam0 << std::endl;
                    //std::cerr << "Phi=" << eqs.Phi   << std::endl;

                    // compute the Newton's step
                    tao::mmul_rtrn(PhiQ,Phi,Q);
                    if(!LU<double>::build(PhiQ))
                    {
                        throw exception("%s: singular composition...",fn);
                    }

                    tao::neg(dV,Gamma);
                    LU<double>::solve(PhiQ,dV);
                    std::cerr << "dV=" << dV << std::endl;

                    triplet<double> xx = { 0.0,  shrink,    -1 };
                    triplet<double> ff = { gam0, F(shrink), -1 };
                    bracket<double>::expand(F, xx, ff);
                    optimize1D<double>::run(F, xx, ff, 0.0);
                    const double alpha = max_of<double>(xx.b,0);
                    const double gam1  = F(alpha);
                    std::cerr << "gam1=" << gam1 << " //@" << xx.b << std::endl;
                    std::cerr << "Ctmp=" << Ctmp << std::endl;
                    if(gam1<gam0)
                    {
                        gam0 = gam1;
                        tao::set(C,Ctmp);
                        tao::set(V,Vtmp);
                        eqs.updatePhi(C);
                        goto LOOP;
                    }
                    // else keep old value



                }


                double call_E(const array<double> &Vtry)
                {
                    gen_C(Ctmp,Vtry);
                    double ans = 0;
                    for(size_t i=M;i>0;--i)
                    {
                        if(eqs.active[i])
                        {
                            const double Ci = Ctmp[i];
                            if(Ci<=0) ans += Ci*Ci;
                        }
                    }
                    return ans*0.5;
                }

                void call_G(array<double> &Grad, const array<double> &Vtry)
                {
                    gen_C(Ctmp,Vtry);
                    for(size_t i=M;i>0;--i)
                    {
                        beta[i] = 0;
                        if(eqs.active[i])
                        {
                            const double Ci = Ctmp[i];
                            if(Ci<0)
                            {
                                beta[i] = Ci;
                            }
                        }
                    }
                    tao::mul(Grad,Q,beta);
                }

                inline void gen_C(array<double>       &Cout,
                                  const array<double> &Vtry) throw()
                {
                    tao::set(Cout,Cstar);
                    tao::mul_add_trn(Cout,Q,Vtry);
                }

                // move V to get the most valid composition
                inline
                void moveV(array<double> &Ctry,
                           array<double> &Vtry)
                {
                    (void)cgrad<double>::optimize(E,
                                                  G,
                                                  Vtry,
                                                  used,
                                                  0.0);
                    gen_C(Ctry,Vtry);
                    eqs.validate(Ctry);
                    tao::mul(Vtry,Q,Ctry);
                }

                inline
                double call_F(double x)
                {
                    tao::setprobe(Vtmp,V,x,dV);
                    moveV(Ctmp,Vtmp);
                    eqs.updateGamma(Ctmp);
                    return 0.5*tao::norm_sq(eqs.Gamma);
                }

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(BootInit);
            };

        }


        void boot:: initialize( array<double> &C0, equilibria &eqs, const double t)
        {
            assert(C0.size()>=eqs.M);
            BootInit ini(eqs,constraints,t);
            for(size_t i=eqs.M;i>0;--i) C0[i] = ini.C[i];
        }



    }

}

