#include "yocto/ink/pixmap.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;

YOCTO_UNIT_TEST_IMPL(bitmap)
{
    Ink::Bitmap bmp(4,200,100);
    Ink::Pixmap<double> pxm(200,100);
    
}
YOCTO_UNIT_TEST_DONE()

