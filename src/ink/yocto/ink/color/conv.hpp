
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
            

        };
    }
}

#endif

