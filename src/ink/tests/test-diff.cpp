#include "yocto/ink/ops/differential.hpp"
#include "yocto/ink/color/conv.hpp"
#include "yocto/ink/image.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ink/color/ramp/cold_to_cold.hpp"
#include "yocto/ink/ops/mapper.hpp"


using namespace yocto;
using namespace Ink;

YOCTO_UNIT_TEST_IMPL(diff)
{
    YOCTO_IMG();
    std::cerr << "sizeof(Domain)=" << sizeof(Domain) << std::endl;
    std::cerr << "sizeof(cslot) =" << sizeof(cslot)  << std::endl;

    Engine::SharedServer seqSrv( new threading::seq_server() );
    Engine::SharedServer parSrv( new threading::par_server() );
    ramp_cold_to_cold rmp(0,math::numeric<float>::pi);
    Mapper mapper;
    if(argc>1)
    {
        const PixmapRGB pxm3( IMG.loadRGB(argv[1],NULL) );
        IMG.save(pxm3,"img.png",NULL);
        
        Engine          par(pxm3,parSrv);
        Engine          seq(pxm3,seqSrv);

        Gradient    grad(pxm3);

        grad.compute(pxm3,Convert::RGB2F,seq);
        std::cerr << "gn_max=" << grad.gn_max << std::endl;
        grad.compute(pxm3,Convert::RGB2F,par);
        std::cerr << "gn_max=" << grad.gn_max << std::endl;
        IMG.save(grad.gn,"grad.png",NULL);

        PixmapRGB               angles(pxm3.w,pxm3.h);

        functor<RGB,TL1(float)> a2rgb( &rmp, & ramp::toRGB );
        mapper(angles,grad.ga,a2rgb,par);
        IMG.save(angles, "angle.png", NULL);


        mapper(angles,grad.gn,angles,Convert::scaleRGB,par);
        IMG.save(angles, "angles.png", NULL);

    }
    
}
YOCTO_UNIT_TEST_DONE()


