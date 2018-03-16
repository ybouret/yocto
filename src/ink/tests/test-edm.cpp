
#include "yocto/ink/ops/histogram.hpp"
#include "yocto/ink/ops/edm.hpp"
#include "yocto/ink/color/conv.hpp"
#include "yocto/ink/image.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/ink/color/ramp/cold_to_very_hot.hpp"
#include "yocto/ink/ops/filter.hpp"

using namespace yocto;
using namespace Ink;

YOCTO_UNIT_TEST_IMPL(edm)
{
    YOCTO_IMG();

    Engine::SharedServer seqSrv( new threading::seq_server() );
    Engine::SharedServer parSrv( new threading::par_server() );

    Histogram H;

    if(argc>1)
    {
        const PixmapRGB pxm( IMG.loadRGB(argv[1],NULL) );
        IMG.save(pxm,"img.png",NULL);
        Engine    par(pxm,parSrv);
        Histogram H;
        PixmapRGB fg(pxm.w,pxm.h);
        H.foreground(fg,pxm, Convert::RGB2U, par);
        IMG.save(fg,"fg.png",NULL);

        EDM edm(pxm.w,pxm.h);
        
        edm.compute(fg,par);
        ramp_cold_to_very_hot cr(0,edm.ed_max);
        IMG.save("edm.png", edm, cr, NULL);

        Filter F;
        F.fillHoles(fg,par);
        IMG.save(fg,"fg1.png",NULL);

    }

}
YOCTO_UNIT_TEST_DONE()




