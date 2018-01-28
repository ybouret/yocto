
#include "yocto/ink/image.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;


YOCTO_UNIT_TEST_IMPL(fmt)
{
    Ink::Image       &IMG = Ink::Image::instance();
    IMG.Init();
    IMG.display();
    
    for(int iarg=1;iarg<argc;++iarg)
    {
        const string    fn = argv[iarg];
        Ink::PixmapRGBA pxm4(   IMG.loadRGBA(fn,NULL) );
        Ink::PixmapRGB  pxm3(   IMG.loadRGB(fn,NULL) );
        Ink::PixmapF    pxmf(   IMG.loadGSF(fn,NULL) );
        Ink::PixmapU    pxmu(   IMG.loadGSU(fn,NULL) );
        Ink::PixmapYUV  pxmyuv( IMG.loadYUV(fn,NULL) );
    }
    
}
YOCTO_UNIT_TEST_DONE()

