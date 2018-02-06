#include "yocto/ink/ops/channels.hpp"
#include "yocto/ink/ops/mapper.hpp"

#include "yocto/ink/image.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/sys/wtime.hpp"

using namespace yocto;
using namespace Ink;

YOCTO_UNIT_TEST_IMPL(ops)
{
    YOCTO_IMG();
    Engine::SharedServer seqSrv( new threading::seq_server() );
    Engine::SharedServer parSrv( new threading::par_server() );


    Channels channels;
    wtime chrono;
    chrono.start();
    for(int iarg=1;iarg<argc;++iarg)
    {
        const string     filename = argv[iarg];
        const PixmapRGBA img4( IMG.loadRGBA(filename,NULL) );
        Engine           seq(img4,seqSrv);
        Engine           par(img4,parSrv);
        const unit_t     w = img4.w;
        const unit_t     h = img4.h;
        Pixmaps<uint8_t> chan(4);
        for(size_t i=0;i<3;++i) chan.append(w,h);

        PixmapU &rr = *chan[1];
        red2rgba toRed;
        PixmapU &gg = *chan[2];
        green2rgba toGreen;
        PixmapU &bb = *chan[3];
        blue2rgba toBlue;

        IMG.save( img4, vformat("a%d.png",iarg), NULL );

        chan.ldz();

        double split_seq = 0;
        YOCTO_WTIME(split_seq,channels.split(img4,chan,seq));

        IMG.save( vformat("r%d-seq.png",iarg),rr,toRed,NULL);
        IMG.save( vformat("g%d-seq.png",iarg),gg,toGreen,NULL);
        IMG.save( vformat("b%d-seq.png",iarg),bb,toBlue,NULL);

        chan.ldz();
        double split_par = 0;
        YOCTO_WTIME(split_par,channels.split(img4,chan,par));
        std::cerr << "split_par=" << split_par << "/" << "split_seq=" << split_seq << std::endl;
        std::cerr << "Efficiency: " << ipso::efficiency(split_seq/split_par,par.size()) << std::endl;
        
        IMG.save( vformat("r%d-par.png",iarg),rr,toRed,NULL);
        IMG.save( vformat("g%d-par.png",iarg),gg,toGreen,NULL);
        IMG.save( vformat("b%d-par.png",iarg),bb,toBlue,NULL);

        PixmapRGB img3(w,h);
        img3.ldz();
        double merge_seq = 0;
        YOCTO_WTIME(merge_seq,channels.merge(img3,chan,seq));
        IMG.save( img3, vformat("merge%d-seq.png",iarg), NULL);

        img3.ldz();
        double merge_par = 0;
        YOCTO_WTIME(merge_par,channels.merge(img3,chan,par));
        IMG.save( img3, vformat("merge%d-par.png",iarg), NULL);
        std::cerr << "merge_par=" << merge_par << "/" << "merge_seq=" << merge_seq << std::endl;
        std::cerr << "Efficiency: " << ipso::efficiency(merge_seq/merge_par,par.size()) << std::endl;


        PixmapYUV      yuv(img4, YUV::fromRGBA);
        Pixmaps<float> fch(3,w,h);
        yuv_u2rgba U2RGBA;
        yuv_v2rgba V2RGBA;
        channels.split(yuv,fch,par);
        IMG.save( *fch[1], vformat("y%d.png",iarg), NULL);
        IMG.save(vformat("u%d.png",iarg), *fch[2], U2RGBA, NULL);
        IMG.save(vformat("v%d.png",iarg), *fch[3], V2RGBA, NULL);

        Mapper mapper;
        PixmapRGB img3b(w,h);
        mapper(img3b,img3,Mapper::Copy<RGB,RGB>,par);
        mapper(yuv,img3,YUV::fromRGB,par);

    }
}
YOCTO_UNIT_TEST_DONE()

