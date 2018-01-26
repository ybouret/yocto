#include "yocto/ink/pixmaps.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;

#define __ALEA_SIZE 10+alea.leq(900),10+alea.leq(900)

YOCTO_UNIT_TEST_IMPL(pixmaps)
{
    Ink::PixmapF pxmf(__ALEA_SIZE);
    Ink::Pixmap3 pxm3(__ALEA_SIZE);
    Ink::Pixmap4 pxm4(__ALEA_SIZE);

    Ink::PixmapF pxmf2( pxm3, Ink::Convert::RGB2F );

}
YOCTO_UNIT_TEST_DONE()

