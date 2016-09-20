#include "yocto/gfx/image.hpp"
#include "yocto/gfx/image/jpeg.hpp"
#include "yocto/gfx/image/png.hpp"
#include "yocto/gfx/image/tiff.hpp"

#include "yocto/gfx/ops/edges.hpp"
#include "yocto/string/conv.hpp"

#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace gfx;

YOCTO_UNIT_TEST_IMPL(detect)
{

    YOCTO_GFX_DECL_FORMAT(jpeg);
    YOCTO_GFX_DECL_FORMAT(png);
    YOCTO_GFX_DECL_FORMAT(tiff);

    imageIO          &IMG = image::instance();
    if(argc>1)
    {
        const string  filename = argv[1];
        std::cerr << "-- Loading " << filename << std::endl;
        const pixmapf img0( IMG.loadf(filename,NULL) );

        IMG.save("img0.png",img0,0);

        const unit_t w=img0.w;
        const unit_t h=img0.h;
        xpatches     xps(img0,true);
        EdgeDetector ED(w,h);

        float sig = 1.4f;
        if(argc>2)
        {
            sig = strconv::to<float>(argv[2],"sig");
        }

        pixmapf imgf(img0);
        if(sig>0)
        {
            const stencil_gauss   g5(2,sig);
            stencil::dispatcher   dsp(w,h);
            std::cerr << "-- Gauss5x5,sigma=" << sig << std::endl;
            dsp(g5,imgf,img0,xps);
        }
        else
        {
            std::cerr << "-- No filter..." << std::endl;
        }

        IMG.save("imgf.png",imgf,0);

        std::cerr << "Simple Intensity Map" << std::endl;
        const stencil_scharr_x5 gx;
        const stencil_scharr_y5 gy;

        ED.build_instensity_from(imgf,gx,gy,xps);
        IMG.save("img-intensity.png",ED,0);
        
    }
    
}
YOCTO_UNIT_TEST_DONE()
