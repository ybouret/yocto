
#include "yocto/ink/image.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;


YOCTO_UNIT_TEST_IMPL(fmt)
{
    static const char *exts[] = { "jpg", "png", "tif" };
    
    YOCTO_IMG();
    IMAGES.display();

    for(int iarg=1;iarg<argc;++iarg)
    {
        const string    fn = argv[iarg];
        Ink::PixmapRGBA pxm4(   IMG.loadRGBA(fn,NULL) );
        Ink::PixmapRGB  pxm3(   IMG.loadRGB(fn,NULL) );
        Ink::PixmapF    pxmf(   IMG.loadGSF(fn,NULL) );
        Ink::PixmapU    pxmu(   IMG.loadGSU(fn,NULL) );
        Ink::PixmapYUV  pxmyuv( IMG.loadYUV(fn,NULL) );

        const string root_name = vformat("loaded%d-",iarg);

        {
            const string base_name = root_name + "rgba.ppm";
            pxm4.save(base_name);
        }

        {
            const string base_name = root_name + "rgb.ppm";
            pxm3.save(base_name);
        }




        for(size_t i=0;i<sizeof(exts)/sizeof(exts[0]);++i)
        {
            {
                const string base_name = root_name + "rgba." + exts[i];
                IMG.save(pxm4,base_name,NULL);
            }

            {
                const string base_name = root_name + "rgb." + exts[i];
                IMG.save(pxm3,base_name,NULL);
            }
#if 1
            {
                const string base_name = root_name + "f." + exts[i];
                IMG.save(pxmf,base_name,NULL);
            }

            {
                const string base_name = root_name + "u." + exts[i];
                IMG.save(pxmu,base_name,NULL);
            }

            {
                const string base_name = root_name + "yuv." + exts[i];
                IMG.save(pxmyuv,base_name,NULL);
            }
#endif
        }

    }
    
}
YOCTO_UNIT_TEST_DONE()

