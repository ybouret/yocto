#ifndef YOCTO_INK_PIXMAPS_INCLUDED
#define YOCTO_INK_PIXMAPS_INCLUDED 1

#include "yocto/ink/color/conv.hpp"
#include "yocto/ink/pixmap.hpp"
#include "yocto/ink/color/yuv.hpp"

namespace yocto
{
    namespace Ink
    {
        typedef Pixmap<float>   PixmapF;
        typedef Pixmap<uint8_t> Pixmap1;
        typedef Pixmap<RGB>     Pixmap3;
        typedef Pixmap<RGBA>    Pixmap4;
        typedef Pixmap<YUV>     YUVmap;
    }
}
#endif

