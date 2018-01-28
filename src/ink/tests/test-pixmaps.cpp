#include "yocto/ink/pixmaps.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;

#define __ALEA_SIZE 10+alea.leq(900),10+alea.leq(900)

YOCTO_UNIT_TEST_IMPL(pixmaps)
{
    Ink::PixmapF    pxmf(__ALEA_SIZE);
    Ink::PixmapRGB  pxm3(__ALEA_SIZE);
    Ink::PixmapRGBA pxm4(__ALEA_SIZE);

    Ink::PixmapF   pxmf2( pxm3, Ink::Convert::RGB2F );
    Ink::PixmapYUV yuv1(  pxm3, Ink::YUV::fromRGB   );
}
YOCTO_UNIT_TEST_DONE()

