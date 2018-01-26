#ifndef YOCTO_INK_PIXMAPS_INCLUDED
#define YOCTO_INK_PIXMAPS_INCLUDED 1

#include "yocto/ink/color/rgb.hpp"
#include "yocto/ink/pixmap.hpp"

namespace yocto
{
    namespace Ink
    {
        typedef Pixmap<float>  PixmapF;
        typedef Pixmap<RGB>    Pixmap3;
        typedef Pixmap<RGBA>   Pixmap4;
    }
}
#endif

