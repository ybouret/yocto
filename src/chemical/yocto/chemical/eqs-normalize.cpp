#include "yocto/chemical/equilibria.hpp"
#include "yocto/math/core/lu.hpp"
#include "yocto/math/core/tao.hpp"

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
                C[j] = C0[j];
            }

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

            for(size_t j=M;j>0;--j)
            {
                C0[j] = C[j];
            }

            
        }
    }

}

