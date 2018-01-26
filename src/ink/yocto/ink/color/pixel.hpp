#ifndef YOCTO_INK_COLOR_PIXEL_INCLUDED
#define YOCTO_INK_COLOR_PIXEL_INCLUDED 1

#include "yocto/ink/types.hpp"

namespace yocto
{

    namespace Ink
    {
        template <typename T>
        struct Pixel
        {
            static T zero;
            static T opaque;
        };
    }
}

#endif

