#include "yocto/alchemy/boot.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/svd.hpp"
#include "yocto/math/core/lu.hpp"

#include "yocto/ios/ocstream.hpp"
#include "yocto/math/opt/cgrad.hpp"



namespace yocto
{
    using namespace math;

    namespace alchemy
    {

        static const char fn[] = "boot::solution: ";
        typedef svd<double> SVD;

        class BootSol
        {
        public:
            equilibria    &eqs;
            const library &lib;
            const size_t   M;
            const size_t   N;
            const size_t   Nc;
            matrix<double> P;
            matrix<double> Q;
            vector<double> Lam;
            vector<double> Cstar;
            vector<double> Ctemp;
            vector<double> V;
            vector<double> Vtemp;

            numeric<double>::function F;

            inline BootSol(equilibria     &usr_eqs,
                           const library  &usr_lib,
                           array<boot::constraint::pointer> &cst,
                           const double t ) :
            eqs(usr_eqs),
            lib(usr_lib),
            M(lib.size()),
            N(eqs.size()),
            Nc(cst.size()),
            P(),
            Lam(),
            Cstar(),
            Ctemp(),
            F( this, & BootSol :: call_F )
            {
                if(Nc+N!=M)
                    throw exception("%s#constraints=%u + #equilibria=%u != #species=%u", fn, unsigned(Nc), unsigned(N), unsigned(M) );

                // special cases


                // generic case
                P.make(Nc,M);
                Q.make(N,M);
                Lam.make(Nc);
                Cstar.make(M);
                Ctemp.make(M);
                V.make(N);
                Vtemp.make(N);

                // create linear part
                for(size_t i=1;i<=Nc;++i)
                {
                    const boot::constraint &cc = *cst[i];
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

                // compute Cstar = P'*inv(P*P')*Lam
                {
                    matrix<double> P2(Nc);
                    tao::mmul_rtrn(P2,P,P);
                    std::cerr << "P2=" << P2 << std::endl;
                    if( !LU<double>::build(P2) )
                    {
                        throw exception("%ssingular set of constraints",fn);
                    }
                    vector<double> U(Nc);
                    tao::neg(U,Lam);
                    LU<double>::solve(P2,U);
                    tao::mul_trn(Cstar,P,Lam);
                }
                std::cerr << "Cstar=" << Cstar << std::endl;

                // compute orthonornal matrix
                if( !SVD::orthonormal(Q,P) )
                {
                    throw exception("%sunable to build orthonormal matrix!",fn);
                }
                SVD::truncate(Q);
                std::cerr << "Q=" << Q << std::endl;

                eqs.computeK(t);


               

            }

        


            double call_F(double alpha)
            {
                for(size_t j=M;j>0;--j)
                {
                    if(eqs.active[j])
                    {
                        Ctemp[j] = alpha;
                    }
                    else
                    {
                        Ctemp[j] = 0;
                    }
                }
                eqs.updateGamma(Ctemp);
                return 0.5 * tao::norm_sq(eqs.Gamma);
                //return eqs.Gamma2Value();
            }

            inline void gen_C(array<double>       &Ctry,
                              const array<double> &Vtry) const throw()
            {
                tao::set(Ctry,Cstar);
                tao::mul_add_trn(Ctry,Q,Vtry);
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(BootSol);
        };



        void boot::solution(array<double> &C0,
                            equilibria    &eqs,
                            const double t)
        {

            BootSol sol(eqs, *(eqs.pLib), constraints, t);


        }

    }
}


