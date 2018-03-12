
#include "yocto/ink/ops/stencil.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/ink/image.hpp"

using namespace yocto;
using namespace Ink;


static inline
void run_stencil(Stencil           &sten,
                 const Pixmap<RGB> &img,
                 Engine            &eng)
{
    YOCTO_IMG_IO();
    sten.display();
    
    Pixmap<RGB> pxm(img.w,img.h);
    sten.compile();

    sten.apply(pxm,img,eng,true);
    {
        const string fn = "img3-" + sten.name + ".png";
        IMG.save(pxm,fn, NULL);
    }

    const Pixmap<float> img_gs(img,Convert::RGB2F);
    Pixmap<float>       pxm_gs(img.w,img.h);
    IMG.save(img_gs, "imgf.png", NULL);
    sten.apply(pxm_gs,img_gs,eng,true);
    {
        const string fn = "imgf-" + sten.name + ".png";
        IMG.save(pxm_gs,fn, NULL);
    }

    const Pixmap<uint8_t> img1(img,Convert::RGB2U);
    Pixmap<uint8_t>       pxm1(img.w,img.h);
    IMG.save(img1, "img1.png", NULL);
    sten.apply(pxm1,img1,eng,true);
    {
        const string fn = "img1-" + sten.name + ".png";
        IMG.save(pxm1,fn, NULL);
    }



}

YOCTO_UNIT_TEST_IMPL(stencil)
{


    YOCTO_IMG();
    Engine::SharedServer parSrv( new threading::par_server() );
    Engine::SharedServer seqSrv( new threading::seq_server() );

    Stencil sten1x1("1x1",0,0);
    Stencil sten3x1("3x1",1,0);
    Stencil sten1x3("1x3",0,1);

    sten1x1.ld(1);
    sten3x1.ld(1);
    sten1x3.ld(1);
    Sobel3Y  sobel3y;
    Sobel3X  sobel3x;
    Scharr3Y scharr3y;
    Scharr3X scharr3x;

    sobel3y.print();
    sobel3x.print();
    scharr3y.print();
    scharr3x.print();

    if(argc>1)
    {
        const Pixmap<RGB> img3( IMG.loadRGB(argv[1],NULL) );
        IMG.save(img3, "img3.png", NULL);
        Engine            par(img3,parSrv);
        Engine            seq(img3,seqSrv);



        Pixmap<RGB> pxm(img3.w,img3.h);

        Engine &eng = seq;
#if 1
        run_stencil(sten1x1,img3,eng);
        run_stencil(sten3x1,img3,eng);
        run_stencil(sten1x3,img3,eng);
        run_stencil(sobel3y,img3,eng);
        run_stencil(sobel3x,img3,eng);
        run_stencil(scharr3y,img3,eng);
        run_stencil(scharr3x,img3,eng);
        run_stencil(sobel3y,img3,eng);
#endif

    }

}
YOCTO_UNIT_TEST_DONE()

