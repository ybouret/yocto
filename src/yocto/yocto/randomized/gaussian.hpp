#ifndef YOCTO_RANDOMIZED_GAUSSIAN_INCLUDED
#define YOCTO_RANDOMIZED_GAUSSIAN_INCLUDED 1

#include "yocto/randomized/bits.hpp"
#include "yocto/math/types.hpp"
#include "yocto/math/types.hxx"

namespace yocto
{

    namespace Randomized
    {
        template <typename T, typename BITS>
        class GaussianGenerator : public BITS
        {
        public:
            typedef T type;
            
            explicit GaussianGenerator() throw() :
            BITS(),
            generate(true),
            oldValue(0)
            {}

            virtual ~GaussianGenerator() throw() {}

            //! using the Box-Muller transform
            inline T operator()(void) throw()
            {
                static const T eps = math::numeric<T>::minimum;
                static const T amp = math::numeric<T>::two_pi;

                if(generate)
                {
                    generate = false;
                PROBE:
                    {
                        const T u1 = this->template to<T>();
                        if(u1<=eps) goto PROBE;
                        const T u2 = this->template to<T>();
                        const T l  = -math::Log(u1);
                        const T a  = amp * u2;
                        const T s  = math::Sqrt(l+l);
                        oldValue   = s*math::Cos(a);
                        return       s*math::Sin(a);
                    }
                }
                else
                {
                    generate = true;
                    return oldValue;
                }
            }
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(GaussianGenerator);
            bool generate;
            T    oldValue;
        };
    }
}

#endif

