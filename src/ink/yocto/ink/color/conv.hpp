
#ifndef YOCTO_INK_COLOR_CONV_INCLUDED
#define YOCTO_INK_COLOR_CONV_INCLUDED 1

#include "yocto/ink/color/rgb.hpp"

namespace yocto
{
    namespace Ink
    {
        struct Convert
        {
            static inline float RGB2F(const RGB &col)  throw()
            {
                return Core::GreyScaleF(col.r, col.g, col.b);

            }

            static inline float RGBA2F(const RGBA &col) throw()
            {
                return Core::GreyScaleF(col.r, col.g, col.b)*Core::uFloat[col.a];
            }
            
            template <typename T,typename U>
            static inline T Copy(const U &u) throw() { return T(u); }

            static inline uint8_t RGB2Byte(const RGB &C) throw()
            {
                return YOCTO_INK_F2B(Core::uFloatR[C.r] + Core::uFloatG[C.g] + Core::uFloatB[C.b]);
            }

            static inline uint8_t RGB2BW(const RGB &C) throw()
            {
                const size_t sum = size_t(C.r) + size_t(C.g) + size_t(C.b);
                return uint8_t(sum/3);
            }

            static inline uint8_t Float2Byte(const float f) throw()
            {
                if(f<=0)
                {
                    return 0;
                }
                else
                {
                    if(f>=1)
                    {
                        return 255;
                    }
                    else
                    {
                        return YOCTO_INK_F2B(f);
                    }

                }
            }

        };
    }
}

#endif

