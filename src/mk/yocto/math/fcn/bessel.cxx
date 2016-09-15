#include "yocto/math/ztype.hpp"
#include "yocto/math/fcn/bessel.hpp"
#include "yocto/math/types.hpp"

namespace yocto
{
    namespace math
    {
        ////////////////////////////////////////////////////////////////////////
        template <>
        real_t bessel<real_t>::j0( real_t x )
        {
            double ax,z;
            double xx,y,ans,ans1,ans2;
            
            if ((ax=fabs(x)) < 8.0)
            {
                y=x*x;
                ans1=57568490574.0+y*(-13362590354.0+y*(651619640.7 +y*(-11214424.18+y*(77392.33017+y*(-184.9052456)))));
                ans2=57568490411.0+y*(1029532985.0+y*(9494680.718 +y*(59272.64853+y*(267.8532712+y*1.0))));
                ans=ans1/ans2;
            }
            else
            {
                z=8.0/ax;
                y=z*z;
                xx=ax-0.785398164;
                ans1=1.0+y*(-0.1098628627e-2+y*(0.2734510407e-4
                                                +y*(-0.2073370639e-5+y*0.2093887211e-6)));
                ans2 = -0.1562499995e-1+y*(0.1430488765e-3
                                           +y*(-0.6911147651e-5+y*(0.7621095161e-6
                                                                   -y*0.934945152e-7)));
                ans=sqrt(0.636619772/ax)*(cos(xx)*ans1-z*sin(xx)*ans2);
            }
            return real_t(ans);
        }
        
        ////////////////////////////////////////////////////////////////////////
        template <>
        real_t bessel<real_t>::y0( real_t x )
        {
            double z;
            double xx,y,ans,ans1,ans2;
            assert(x>0);
            
            if (x < 8.0)
            {
                y=x*x;
                ans1 = -2957821389.0+y*(7062834065.0+y*(-512359803.6 +y*(10879881.29+y*(-86327.92757+y*228.4622733))));
                ans2 = 40076544269.0+y*(745249964.8+y*(7189466.438 +y*(47447.26470+y*(226.1030244+y*1.0))));
                ans  = (ans1/ans2)+0.636619772*bessel<double>::j0(x)*log(x);
            } else {
                z=8.0/x;
                y=z*z;
                xx=x-0.785398164;
                ans1 = 1.0+y*(-0.1098628627e-2+y*(0.2734510407e-4
                                                  +y*(-0.2073370639e-5+y*0.2093887211e-6)));
                ans2 = -0.1562499995e-1+y*(0.1430488765e-3
                                           +y*(-0.6911147651e-5+y*(0.7621095161e-6
                                                                   +y*(-0.934945152e-7))));
                ans = sqrt(0.636619772/x)*(sin(xx)*ans1+z*cos(xx)*ans2);
            }
            return real_t(ans);
        }
        
        ////////////////////////////////////////////////////////////////////////
        template <>
        real_t bessel<real_t>::i0( real_t x )
        {
            double ax,ans;
            double y;
            
            if ((ax=fabs(x)) < 3.75)
            {
                y=x/3.75;
                y*=y;
                ans=1.0+y*(3.5156229+y*(3.0899424+y*(1.2067492
                                                     +y*(0.2659732+y*(0.360768e-1+y*0.45813e-2)))));
            } else {
                y=3.75/ax;
                ans=(exp(ax)/sqrt(ax))*(0.39894228+y*(0.1328592e-1
                                                      +y*(0.225319e-2+y*(-0.157565e-2+y*(0.916281e-2 +y*(-0.2057706e-1+y*(0.2635537e-1+y*(-0.1647633e-1 +y*0.392377e-2))))))));
            }
            return static_cast<real_t>(ans);
        }
        
        ////////////////////////////////////////////////////////////////////////
        template <>
        real_t bessel<real_t>::k0( real_t x )
        {
            assert(x>0);
            double y,ans;
            if (x <= 2.0) {
                y=x*x/4.0;
                ans=(-log(x/2.0)*bessel<double>::i0(x))+(-0.57721566+y*(0.42278420 +y*(0.23069756+y*(0.3488590e-1+y*(0.262698e-2 +y*(0.10750e-3+y*0.74e-5))))));
            } else {
                y=2.0/x;
                ans=(exp(-x)/sqrt(x))*(1.25331414+y*(-0.7832358e-1 +y*(0.2189568e-1+y*(-0.1062446e-1+y*(0.587872e-2 +y*(-0.251540e-2+y*0.53208e-3))))));
            }
            return real_t(ans);
        }
        
    }
}
