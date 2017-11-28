#ifndef YOCTO_RANDOMIZED_BIVARIATE_GAUSSIAN_INCLUDED
#define YOCTO_RANDOMIZED_BIVARIATE_GAUSSIAN_INCLUDED 1

#include "yocto/randomized/bits.hpp"
#include "yocto/code/utils.hpp"
#include "yocto/math/types.hpp"
#include "yocto/math/types.hxx"

namespace yocto 
{

    namespace Randomized
    {

        template <typename T,typename BITS>
        class BivariateGaussian : public BITS
        {
        public:
            const T sig1; //!< sig1^2 = variance of u1
            const T sig2; //!< sig2^2 = variance of u2
            const T corr; //!< corr = var( u1.u2 ) / (sig1*sig2);
            const T xorr; //!< sqrt( 1 - corr^2 )

            inline virtual ~BivariateGaussian( ) throw() {}
            
            inline explicit BivariateGaussian( const T s1, const T s2, const T c12  ) throw() :
            BITS(),
            sig1( max_of<double>(s1,0) ),
            sig2( max_of<double>(s2,0) ),
            corr( clamp<double>(0, c12, 1 ) ),
            xorr( math::Sqrt( T(1.0) - (corr*corr) ) )
            {


            }

            inline void operator()( T &v1, T &v2 ) throw()
            {
                static const T eps = math::numeric<T>::minimum;
                static const T amp = math::numeric<T>::two_pi;
            PROBE:
                {
                    const T u1 = this->template to<T>();
                    if(u1<=eps) goto PROBE;
                    const T u2 = this->template to<T>();
                    const T l  = -math::Log(u1);
                    const T a  = amp * u2;
                    const T s  = math::Sqrt(l+l);
                    const T z1 = s*math::Cos(a);
                    const T z2 = s*math::Sin(a);
                    v1 = sig1 * z1;
                    v2 = sig2 * ( corr * z1 + xorr * z2 );
                }
            }
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(BivariateGaussian);
        };

    }

}
#endif
