
#include "yocto/ink/ops/sobel.hpp"
#include "yocto/ink/ops/scharr.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/ink/image.hpp"
#include "yocto/ink/ops/gradients.hpp"
#include "yocto/ink/color/ramp/cold_to_cold.hpp"

#include "yocto/ink/ops/mapper.hpp"

using namespace yocto;
using namespace Ink;

static inline void compute_grads(Gradients           &grad,
                                 const Pixmap<float> &pxm,
                                 const Stencil &dx,
                                 const Stencil &dy,
                                 Engine        &engine)
{
    grad.compute(pxm, dx, dy, engine, true);
 
    YOCTO_IMG_IO();
    const string pfx = "img" + dx.name + "-" + dy.name;
    {
        const string fn  = pfx + ".png";
        IMG.save(grad.value,fn, NULL);
    }
    
    Pixmap<RGB>       ang(pxm.w,pxm.h);
    ramp_cold_to_cold r(-3.14,3.14);
    functor<RGB,TL1(float)> func( &r, & ramp::toRGB);
    Mapper mapper;
    mapper(ang,grad.angle,func,engine);
    mapper(ang,grad.value,ang,Convert::scaleRGB,engine);
    {
        const string fn = "angle-" + pfx + ".png";
        IMG.save(ang,fn,NULL);
    }
    
}

YOCTO_UNIT_TEST_IMPL(grads)
{
    YOCTO_IMG();
    Engine::SharedServer parSrv( new threading::par_server() );

    if(argc>1)
    {
        const Pixmap<float> img( IMG.loadGSF(argv[1],NULL) );
        IMG.save(img, "img.png", NULL);
        Engine            par(img,parSrv);
        
        Gradients grad(img);
        {
            const StencilDX dx;
            const StencilDY dy;
            compute_grads(grad, img, dx, dy, par);
        }
        
        {
            const Sobel3X dx;
            const Sobel3Y dy;
            compute_grads(grad, img, dx, dy, par);
        }
        
        {
            const Sobel5X dx;
            const Sobel5Y dy;
            compute_grads(grad, img, dx, dy, par);
        }
        
        {
            const Scharr3X dx;
            const Scharr3Y dy;
            compute_grads(grad, img, dx, dy, par);
        }
        
        {
            const Scharr5X dx;
            const Scharr5Y dy;
            compute_grads(grad, img, dx, dy, par);
        }
        
    }
    
}
YOCTO_UNIT_TEST_DONE()

