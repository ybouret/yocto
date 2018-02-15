
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
        const size_t    level = H.build(pxm3,Convert::RGB2U,par).threshold();
        PixmapRGB       pxm(pxm3.w,pxm3.h);
        PixmapRGB       tgt(pxm3.w,pxm3.h);
        H.keep(level, Histogram::KeepGEQ, pxm, pxm3, Convert::RGB2U,par);
        IMG.save(pxm,"fg.png",NULL);

        indx2rgba<size_t> blobColors(10);
        Blob      blobs(pxm);
        Particles particles;

        {
            blobs.build(pxm,particles,false);
            blobs.rewrite(particles);
            IMG.save("blob_fg4.png", blobs, blobColors, NULL);
            std::cerr << "#blob_fg4=" << particles.size << std::endl;
            tgt.ldz();
            if(particles.size)
            {
                particles.head->transfer(tgt,pxm3,Convert::Copy<RGB,RGB>);
            }
            IMG.save(tgt, "main_fg4.png", NULL);

            blobs.build(pxm,particles,true);
            blobs.rewrite(particles);
            IMG.save("blob_fg8.png", blobs, blobColors, NULL);
            std::cerr << "#blob_fg8=" << particles.size << std::endl;
            tgt.ldz();
            if(particles.size)
            {
                particles.head->transfer(tgt,pxm3,Convert::Copy<RGB,RGB>);
            }
            IMG.save(tgt, "main_fg8.png", NULL);
        }

        H.keep(level, Histogram::KeepLEQ, pxm, pxm3, Convert::RGB2U,par, true);
        IMG.save(pxm,"bg.png",NULL);
        {
            blobs.build(pxm,particles,false);
            blobs.rewrite(particles);
            IMG.save("blob_bg4.png", blobs, blobColors, NULL);
            std::cerr << "#blob_bg4=" << particles.size << std::endl;

            blobs.build(pxm,particles,true);
            blobs.rewrite(particles);
            IMG.save("blob_bg8.png", blobs, blobColors, NULL);
            std::cerr << "#blob_bg8=" << particles.size << std::endl;
        }
    }

}
YOCTO_UNIT_TEST_DONE()

#include "yocto/ink/ops/filter.hpp"
YOCTO_UNIT_TEST_IMPL(fg)
{
    YOCTO_IMG();
    Engine::SharedServer parSrv( new threading::par_server() );
    Histogram            H;

    if(argc>1)
    {
        // load image
        PixmapRGB img( IMG.loadRGB(argv[1],NULL) );
        Engine    par(img,parSrv);
        IMG.save(img,"img.png",NULL);

        // keep foreground
        //const size_t level = H.build(img,Convert::RGB2U,par).threshold();
        //H.keep(level, Histogram::KeepGEQ, fg, img, Convert::RGB2U,par);
        PixmapRGB    fg(img.w,img.h);
        H.foreground(fg,img,Convert::RGB2U,par);
        IMG.save(fg,"fg0.png",NULL);



        indx2rgba<size_t> blobColors(0);
        Blob      blobs(fg);
        Particles particles;
        blobs.build(fg,particles,true);
        blobs.rewrite(particles);
        IMG.save("blobs0.png", blobs, blobColors, NULL);
        particles.discardSmallerThan(2);
        blobs.rewrite(particles);
        IMG.save("blobs1.png", blobs, blobColors, NULL);

    }
}
YOCTO_UNIT_TEST_DONE()

