#include "yocto/chemical/equilibria.hpp"
#include "yocto/math/core/lu.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/svd.hpp"

namespace yocto
{
    using namespace math;

    namespace chemical
    {
        static const char fn[] = "equilibria.normalize: ";

        void equilibria:: normalize(array<double> &C0, const double t)
        {
            //__________________________________________________________________
            //
            // initialize
            //__________________________________________________________________
            for(size_t j=M;j>0;--j)
            {
                const double Cj = (C[j] = C0[j]);
                if(active[j]&&(Cj<0))
                {
                    equilibrium::database      &db = *this;
                    const equilibrium::pointer &eq = db(j);
                    throw exception("%s: invalid initial [%s]=%g",fn, *(eq->name), Cj);
                }
            }

            initializeGammaAndPhi(C,t);
            size_t cycle=0;
            while(true)
            {
                //______________________________________________________________
                //
                // compute system and full extent
                //______________________________________________________________
                ++cycle;
                //std::cerr << "C    =" << C    << std::endl;
                //std::cerr << "Gamma=" << Gamma << std::endl;
                //std::cerr << "gs   =" << GammaToScalar() << std::endl;
                if(!computeW())
                {
                    throw exception("%ssingular system",fn);
                }
                
                
                tao::neg(xi,Gamma);
                LU<double>::solve(W,xi);
                
                //______________________________________________________________
                //
                // compute expected full new concentration
                //______________________________________________________________
                tao::mul(dC,NuT,xi);
                tao::add(C,dC);
                
                //______________________________________________________________
                //
                // check step
                //______________________________________________________________
                bool bad = false;
                for(size_t j=M;j>0;--j)
                {
                    beta[j] = 0;
                    const double Cj = C[j];
                    if(active[j]&&(Cj<0))
                    {
                        beta[j] = -Cj;
                        bad     = true;
                    }
                }
                if(bad)
                {
                    std::cerr << "C   =" << C    << std::endl;
                    std::cerr << "beta=" << beta << std::endl;
                    if(M>N)
                    {
                        const size_t   dof = M-N;
                        matrix<double> sigma(dof,M);
                        if(!svd<double>::orthonormal(sigma,Nu))
                        {
                            std::cerr << "couldn't build sigma" << std::endl;
                        }
                        //std::cerr << "nu    = " << nu << std::endl;
                        std::cerr << "sigma = " << sigma << std::endl;
                        vector<double> q(dof);
                        tao::mul(q,sigma,beta);
                        std::cerr << "q=" << q << std::endl;
                    }
                    exit(0);
                }
                if(cycle>10) break;
                updateGammaAndPhi(C);
            }
            
            
#if 0
            if(!balance(C))
            {
                throw exception("%sno initial balancing!",fn);
            }

            initializeGammaAndPhi(C,t);
            size_t cycle=0;
            while(true)
            {
                ++cycle;
                // compute system matrix
                std::cerr << "C    =" << C    << std::endl;
                std::cerr << "Gamma=" << Gamma << std::endl;
                if(!computeW())
                {
                    throw exception("%ssingular system",fn);
                }

                // compute extent
                tao::neg(xi,Gamma);
                LU<double>::solve(W,xi);

                // compute dC
                tao::mul(dC,nuT,xi);
                tao::add(C,dC);

                if(!balance(C))
                {
                    throw exception("%s: unable to balance",fn);
                }

                if(cycle>10) break;
                updateGammaAndPhi(C);
            }
#endif
            
            for(size_t j=M;j>0;--j)
            {
                C0[j] = C[j];
            }

            
        }
    }

}

