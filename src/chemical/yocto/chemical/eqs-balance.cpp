#include "yocto/chemical/equilibria.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/opt/minimize.hpp"
#include "yocto/math/opt/bracket.hpp"

namespace yocto
{
    using namespace math;
    
    namespace chemical
    {

        double equilibria:: callE(double alpha)
        {
            tao::setprobe(Ctry, C, alpha, dC);
            double ans = 0.0;
            for(size_t j=M;j>0;--j)
            {
                const double Cj = Ctry[j];
                if(active[j] && (Cj<0) )
                {
                    ans += Cj * Cj;
                }
            }
            return ans*0.5;
        }

        bool equilibria:: balance( array<double> &C0, const double t)
        {
            for(size_t j=M;j>0;--j)
            {
                C[j] = C0[j];
            }

            //__________________________________________________________________
            //
            // Initial Check
            //__________________________________________________________________
            double E0 = 0;
            for(size_t j=M;j>0;--j)
            {
                const double Cj = C[j];
                if(active[j] && (Cj<0) )
                {
                    E0     +=  Cj*Cj;
                    beta[j] = -Cj;
                }
            }
            E0 *= 0.5;
            std::cerr << "E0=" << E0 << std::endl;
            if(E0<=0)
            {
                return true;
            }


            return false;
        }

    }

}

