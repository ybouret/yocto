
#include "yocto/ink/ops/sobel.hpp"
#include "yocto/ink/ops/scharr.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/ink/image.hpp"
#include "yocto/ink/ops/differential.hpp"

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
    Sobel5Y  sobel5y;
    Sobel5X  sobel5x;

    Scharr3Y scharr3y;
    Scharr3X scharr3x;

    Scharr5Y scharr5y;
    Scharr5X scharr5x;

    sobel3y.print();
    sobel3x.print();
    sobel5y.print();
    sobel5x.print();

    scharr3y.print();
    scharr3x.print();
    scharr5y.print();
    scharr5x.print();

    if(argc>1)
    {
        const Pixmap<RGB> img3( IMG.loadRGB(argv[1],NULL) );
        IMG.save(img3, "img3.png", NULL);
        Engine            par(img3,parSrv);
        Engine            seq(img3,seqSrv);



        Pixmap<RGB> pxm(img3.w,img3.h);

        Engine &eng = par;
#if 1
        run_stencil(sten1x1,img3,eng);
        run_stencil(sten3x1,img3,eng);
        run_stencil(sten1x3,img3,eng);
        run_stencil(sobel3y,img3,eng);
        run_stencil(sobel3x,img3,eng);
        run_stencil(scharr3y,img3,eng);
        run_stencil(scharr3x,img3,eng);
        run_stencil(sobel5y,img3,eng);
        run_stencil(sobel5x,img3,eng);
        run_stencil(scharr5y,img3,eng);
        run_stencil(scharr5x,img3,eng);
#endif

    }

}
YOCTO_UNIT_TEST_DONE()

static inline void compute_sgrad(Pixmap<float> &gn,
                                 const Pixmap<float> &img,
                                 const Stencil &sx,
                                 const Stencil &sy)
{
    float gn_max = 0;
    for(unit_t y=img.y_end;y>=0;--y)
    {
        for(unit_t x=img.x_end;x>=0;--x)
        {
            float gx=0,gy=0;
            Stencil::compute(gx, sx, gy, sy, img, x, y);
            const float g = sqrtf(gx*gx+gy*gy);
            gn[y][x] = g;
            gn_max   = max_of(gn_max,g);
        }
    }
    if(gn_max>0)
    {
        for(unit_t y=img.y_end;y>=0;--y)
        {
            for(unit_t x=img.x_end;x>=0;--x)
            {
                gn[y][x] /= gn_max;
            }
        }
    }

}

YOCTO_UNIT_TEST_IMPL(stengrad)
{
    YOCTO_IMG();
    Engine::SharedServer parSrv( new threading::par_server() );

    Sobel3X sobel3x;
    Sobel3Y sobel3y;

    Sobel5X sobel5x;
    Sobel5Y sobel5y;

    Scharr3X scharr3x;
    Scharr3Y scharr3y;

    Scharr5X scharr5x;
    Scharr5Y scharr5y;


    if(argc>1)
    {
        const Pixmap<float> img( IMG.loadGSF(argv[1],NULL) );
        Engine              par(img,parSrv);
        IMG.save(img, "img.png", NULL);

        Gradient grad(img);
        grad.compute(img,Convert::Copy<float,float>, par, true);
        IMG.save(grad.gn,"img-gn.png",NULL);

        compute_sgrad(grad.gn,img,sobel3x,sobel3y);
        IMG.save(grad.gn,"img-sobel3.png",NULL);

        compute_sgrad(grad.gn,img,sobel5x,sobel5y);
        IMG.save(grad.gn,"img-sobel5.png",NULL);

        compute_sgrad(grad.gn,img,scharr3x,scharr3y);
        IMG.save(grad.gn,"img-scharr3.png",NULL);

        compute_sgrad(grad.gn,img,scharr5x,scharr5y);
        IMG.save(grad.gn,"img-scharr5.png",NULL);
    }

}
YOCTO_UNIT_TEST_DONE()
