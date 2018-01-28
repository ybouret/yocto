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
        typedef Pixmap<uint8_t> PixmapU;
        typedef Pixmap<RGB>     PixmapRGB;
        typedef Pixmap<RGBA>    PixmapRGBA;
        typedef Pixmap<YUV>     PixmapYUV;
    }
}
#endif

