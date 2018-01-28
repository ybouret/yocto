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
            static const T zero;
            static const T opaque;
        };
    }
}

#define YOCTO_INK_PIXEL_EXTERN_(TYPE) extern template const TYPE yocto::Ink::Pixel<TYPE>::opaque

#define YOCTO_INK_PIXEL_EXTERN()  \
YOCTO_INK_PIXEL_EXTERN_(uint8_t); \
YOCTO_INK_PIXEL_EXTERN_(float);   \
YOCTO_INK_PIXEL_EXTERN_(double)

#endif

