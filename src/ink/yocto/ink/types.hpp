#ifndef YOCTO_INK_TYPES_INCLUDED
#define YOCTO_INK_TYPES_INCLUDED 1

#include "yocto/ipso/field2d.hpp"

namespace yocto
{
    namespace Ink
    {
        typedef ipso::coord2D         Coord;
        typedef ipso::patch<Coord>    Patch;
    }
}

#endif

