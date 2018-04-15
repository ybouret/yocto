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
            if(!balance(C0))
            {
                throw exception("%sno initial balancing!",fn);
            }

            initializeGammaAndPhi(C0,t);
            size_t cycle=0;
            while(true)
            {
                ++cycle;
                // compute system matrix
                std::cerr << "C0   =" << C0    << std::endl;
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
                for(size_t j=M;j>0;--j)
                {
                    C0[j] += dC[j];
                }

                if(!balance(C0))
                {
                    throw exception("%s: unable to balance",fn);
                }

                if(cycle>10) break;
                updateGammaAndPhi(C0);
            }

            
        }
    }

}

