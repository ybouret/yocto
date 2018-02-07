
#include "yocto/ink/ops/histogram.hpp"
#include "yocto/ink/color/conv.hpp"
#include "yocto/ink/image.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ios/ocstream.hpp"


using namespace yocto;
using namespace Ink;

YOCTO_UNIT_TEST_IMPL(hist)
{
    YOCTO_IMG();

    Engine::SharedServer seqSrv( new threading::seq_server() );
    Engine::SharedServer parSrv( new threading::par_server() );

    Histogram H;

    if(argc>1)
    {
        const PixmapRGB pxm3( IMG.loadRGB(argv[1],NULL) );
        IMG.save(pxm3,"img.png",NULL);
        Engine          par(pxm3,parSrv);
        Engine          seq(pxm3,seqSrv);

        H.build(pxm3,Convert::RGB2Byte,seq);
        {
            ios::wcstream fp("hseq.dat");
            for(size_t i=0;i<Histogram::BINS;++i)
            {
                fp("%u %u\n", unsigned( H.bins[i] ), unsigned( H.hist[i] ) );
            }
        }
        H.build(pxm3,Convert::RGB2Byte,par);
        {
            ios::wcstream fp("hpar.dat");
            for(size_t i=0;i<Histogram::BINS;++i)
            {
                fp("%u %u\n", unsigned( H.bins[i] ), unsigned( H.hist[i] ) );
            }
        }

        const size_t thr = H.threshold();
        std::cerr << "threshold=" << thr << std::endl;

        PixmapRGB tgt(pxm3.w,pxm3.h);
        H.keep(thr,Histogram::KeepLT,  tgt, pxm3,Convert::RGB2Byte,par);
        IMG.save(tgt,"hlt.png",NULL);
        H.keep(thr,Histogram::KeepLEQ, tgt, pxm3,Convert::RGB2Byte,par);
        IMG.save(tgt,"hleq.png",NULL);
        H.keep(thr,Histogram::KeepGT,  tgt, pxm3,Convert::RGB2Byte,par);
        IMG.save(tgt,"hgt.png",NULL);
        H.keep(thr,Histogram::KeepGEQ, tgt, pxm3,Convert::RGB2Byte,par);
        IMG.save(tgt,"hgeq.png",NULL);
    }

}
YOCTO_UNIT_TEST_DONE()


