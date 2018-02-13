
#include "yocto/ink/ops/stencil.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/ink/image.hpp"

using namespace yocto;
using namespace Ink;



YOCTO_UNIT_TEST_IMPL(stencil)
{


    YOCTO_IMG();
    Engine::SharedServer parSrv( new threading::par_server() );
    Engine::SharedServer seqSrv( new threading::seq_server() );

    Stencil sten1x1("1x1",0,0);
    Stencil sten3x1("3x1",1,0);
    Stencil sten1x3("1x3",0,1);

    if(argc>1)
    {
        const Pixmap<RGB> img3( IMG.loadRGB(argv[1],NULL) );
        Engine par(img3,parSrv);
        Engine seq(img3,seqSrv);



        Pixmap<RGB> pxm(img3.w,img3.h);

        sten1x1._apply<RGB,uint8_t,3>(pxm,img3,seq);
        sten3x1._apply<RGB,uint8_t,3>(pxm,img3,seq);
        sten1x3._apply<RGB,uint8_t,3>(pxm,img3,seq);

    }

}
YOCTO_UNIT_TEST_DONE()

