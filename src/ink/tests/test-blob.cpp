
#include "yocto/ink/ops/histogram.hpp"
#include "yocto/ink/ops/blob.hpp"
#include "yocto/ink/color/conv.hpp"
#include "yocto/ink/image.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ios/ocstream.hpp"


using namespace yocto;
using namespace Ink;

YOCTO_UNIT_TEST_IMPL(blob)
{
    YOCTO_IMG();

    Engine::SharedServer seqSrv( new threading::seq_server() );
    Engine::SharedServer parSrv( new threading::par_server() );

    Histogram H;
    std::cerr << "sizeof(Vertex)     =" << sizeof(Vertex)      << std::endl;
    std::cerr << "sizeof(Particle)   =" << sizeof(Particle)    << std::endl;

    if(argc>1)
    {
        const PixmapRGB pxm3( IMG.loadRGB(argv[1],NULL) );
        IMG.save(pxm3,"img.png",NULL);
        Engine          par(pxm3,parSrv);
        const size_t    level = H.build(pxm3,Convert::RGB2Byte,par).threshold();
        PixmapRGB       pxm(pxm3.w,pxm3.h);
        H.keep(level, Histogram::KeepGEQ, pxm, pxm3, Convert::RGB2Byte,par);
        IMG.save(pxm,"fg.png",NULL);

        indx2rgba blobColors(10);
        Blob      blobs(pxm);
        Particles particles;

        {
            blobs.build(pxm,particles,false);
            IMG.save("blob_fg4.png", blobs, blobColors, NULL);
            std::cerr << "#blob_fg4=" << particles.size << std::endl;

            blobs.build(pxm,particles,true);
            IMG.save("blob_fg8.png", blobs, blobColors, NULL);
            std::cerr << "#blob_fg8=" << particles.size << std::endl;
        }

        H.keep(level, Histogram::KeepLEQ, pxm, pxm3, Convert::RGB2Byte,par, true);
        IMG.save(pxm,"bg.png",NULL);
        {
            blobs.build(pxm,particles,false);
            IMG.save("blob_bg4.png", blobs, blobColors, NULL);
            std::cerr << "#blob_bg4=" << particles.size << std::endl;

            blobs.build(pxm,particles,true);
            IMG.save("blob_bg8.png", blobs, blobColors, NULL);
            std::cerr << "#blob_bg8=" << particles.size << std::endl;
        }
    }

}
YOCTO_UNIT_TEST_DONE()


