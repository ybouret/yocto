#include "yocto/ink/ops/channels.hpp"
#include "yocto/ink/ops/map.hpp"

#include "yocto/ink/image.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/sys/wtime.hpp"

using namespace yocto;
using namespace Ink;

YOCTO_UNIT_TEST_IMPL(ops)
{
    YOCTO_IMG();
    SharedServer seq( new threading::seq_server() );
    SharedServer par( new threading::par_server() );


    Channels channels;
    wtime chrono;
    chrono.start();
    for(int iarg=1;iarg<argc;++iarg)
    {
        const string     filename = argv[iarg];
        const PixmapRGBA img4( IMG.loadRGBA(filename,NULL) );
        const Domains    seq_doms(img4,seq);
        const Domains    par_doms(img4,par);
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

        double ell_seq = 0;
        YOCTO_WTIME(ell_seq,channels.split(img4,chan,seq_doms));

        IMG.save( vformat("r%d-seq.png",iarg),rr,toRed,NULL);
        IMG.save( vformat("g%d-seq.png",iarg),gg,toGreen,NULL);
        IMG.save( vformat("b%d-seq.png",iarg),bb,toBlue,NULL);

        chan.ldz();
        double ell_par = 0;
        YOCTO_WTIME(ell_par,channels.split(img4,chan,par_doms));
        std::cerr << "ell_par=" << ell_par << "/" << "ell_seq=" << ell_seq << std::endl;

        IMG.save( vformat("r%d-par.png",iarg),rr,toRed,NULL);
        IMG.save( vformat("g%d-par.png",iarg),gg,toGreen,NULL);
        IMG.save( vformat("b%d-par.png",iarg),bb,toBlue,NULL);

        PixmapRGB img3(w,h);
        img3.ldz();
        double merge_seq = 0;
        YOCTO_WTIME(merge_seq,channels.merge(img3,chan,seq_doms));
        IMG.save( img3, vformat("merge%d-seq.png",iarg), NULL);

        img3.ldz();
        double merge_par = 0;
        YOCTO_WTIME(merge_par,channels.merge(img3,chan,par_doms));
        IMG.save( img3, vformat("merge%d-par.png",iarg), NULL);

        PixmapYUV      yuv(img4, YUV::fromRGBA);
        Pixmaps<float> fch(3,w,h);
        channels.split(yuv,fch,par_doms);
        IMG.save( *fch[1], vformat("y%d.png",iarg), NULL);
        IMG.save( *fch[2], vformat("u%d.png",iarg), NULL);
        IMG.save( *fch[3], vformat("v%d.png",iarg), NULL);

        Map mapper;
        PixmapRGB img3b(w,h);
        mapper(img3b,img3,Map::Copy<RGB,RGB>,par_doms);
        mapper(yuv,img3,YUV::fromRGB,par_doms);

    }
}
YOCTO_UNIT_TEST_DONE()

