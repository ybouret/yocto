#include "yocto/alchemy/boot.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/svd.hpp"

namespace yocto
{
    using namespace math;

    namespace alchemy
    {

        static const char fn[] = "boot::solution: ";
        typedef svd<double> SVD;

        void boot::solution(array<double> &C0,
                            equilibria    &eqs,
                            const double t)
        {
            const size_t Nc = constraints.size();
            const size_t M  = eqs.pLib->size();
            const size_t N  = eqs.size();
            if(Nc+N!=M)
                throw exception("%s#constraints=%u + #equilibria=%u != #species=%u", fn, unsigned(Nc), unsigned(N), unsigned(M) );

            // special cases


            // generic case
            matrix<double> P(Nc,M);
            vector<double> Lam(Nc);

            for(size_t i=1;i<=Nc;++i)
            {
                const boot::constraint &cc = *constraints[i];
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

            // TODO: check rank !

            matrix<double> U(M,M);
            vector<double> C(M);
            matrix<double> &Phi = eqs.Phi;
            matrix<double> V(M,M);
            vector<double> W(M);
            vector<double> F(M);
            vector<double> dC(M);
            array<double> &Gam = eqs.Gamma;

            tao::ld(C, 0.0001);

            eqs.computePhi(C,0.0);
            std::cerr << "C="   << C   << std::endl;
            std::cerr << "Phi=" << Phi << std::endl;
            {
                size_t j=1;
                for(size_t i=1;i<=Nc;++i,++j)
                {
                    tao::set(U[j],P[i]);
                }
                for(size_t i=1;i<=N;++i,++j)
                {
                    tao::set(U[j],Phi[i]);
                }
                
            }

            std::cerr << "J=" << U << std::endl;
            if( ! SVD::build(U,W,V) )
            {
                throw exception("%scannot build SVD",fn);
            }
            std::cerr << "W0=" << W << std::endl;
            const size_t dimK = svd<double>::truncate(W);
            std::cerr << "W=" << W << std::endl;
            std::cerr << "dimK=" << dimK << std::endl;

            {
                size_t j=1;
                for(size_t i=1;i<=Nc;++i,++j)
                {
                    F[j] = Lam[i];
                }
                for(size_t i=1;i<=N;++i,++j)
                {
                    F[j] = Gam[i];
                }
            }
            std::cerr << "F=" << F << std::endl;
            tao::neg(F,F);
            SVD::solve(U,W,V,F,dC);
            std::cerr << "dC=" << dC << std::endl;
            pLib->display(std::cerr,dC);

        }

    }
}


