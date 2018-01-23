#ifndef YOCTO_INK_TYPES_INCLUDED
#define YOCTO_INK_TYPES_INCLUDED 1

#include "yocto/ipso/patch.hpp"

namespace yocto
{
    namespace Ink
    {
        typedef ipso::coord2D         coord_t;
        typedef ipso::patch<coord_t>  patch_t;
    }
}

#endif

