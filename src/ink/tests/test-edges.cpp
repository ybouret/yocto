
#include "yocto/ink/ops/edges.hpp"
#include "yocto/ink/color/conv.hpp"
#include "yocto/ink/color/ramp/cold_to_very_hot.hpp"
#include "yocto/ink/color/ramp/cold_to_cold.hpp"
#include "yocto/ink/color/ramp/grey.hpp"
#include "yocto/ink/ops/mapper.hpp"
#include "yocto/ink/image.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ios/ocstream.hpp"


using namespace yocto;
using namespace Ink;

YOCTO_UNIT_TEST_IMPL(edges)
{
    YOCTO_IMG();

    if(false)
    {
        ios::wcstream fp("probe.dat");
        const float radius = 1.0;
        for(float theta=0;theta<=6.3f;theta+=0.05f)
        {
            const float x  = radius * cos(theta);
            const float y  = radius * sin(theta);
            const float dx = floorf( x + 0.5f );
            const float dy = floorf( y + 0.5f );
            fp("%g %g\n%g %g\n\n", x, y, dx, dy);
        }

    }

    Engine::SharedServer seqSrv( new threading::seq_server() );
    Engine::SharedServer parSrv( new threading::par_server() );

    Mapper            mapper;
    BlurOn<5>         g5x5(1.4f);
    ramp_cold_to_cold armp(-math::numeric<float>::pi,math::numeric<float>::pi);
    std::cerr << "atan2(0,1)=" << math::Atan2(0.0f,1.0f) << std::endl;
    if(argc>1)
    {
        const PixmapRGB pxm3( IMG.loadRGB(argv[1],NULL) );
        Pixmap<float>   pxmf( pxm3, Convert::RGB2F );
        IMG.save(pxm3, "img3.png", NULL);
        IMG.save(pxmf, "imgf.png", NULL);
        Engine par(pxm3,parSrv);
        Edges  edges(pxm3.w,pxm3.h);
        g5x5.apply(edges.S,pxmf,par);
        IMG.save(edges.S, "edgesS.png", NULL);
        edges.computeGradient(par);

        //ramp_cold_to_very_hot grmp(0.0f, edges.Gmax );
        ramp_grey  grmp(0.0f, edges.Gmax );

        IMG.save("gradI.png", edges.G, grmp, NULL);
        IMG.save("gradA.png", edges.A, armp, NULL);

        edges.keepLocalMaxima(par);
        IMG.save(edges.M,"gradM.png",NULL);

    }

}
YOCTO_UNIT_TEST_DONE()

