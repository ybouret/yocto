#include "yocto/ink/ops/channels.hpp"
#include "yocto/ink/image.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/sequence/vector.hpp"

using namespace yocto;
using namespace Ink;

YOCTO_UNIT_TEST_IMPL(ops)
{
    YOCTO_IMG();

    threading::seq_server seq;
    threading::par_server par;

    Channels channels;

    for(int iarg=1;iarg<argc;++iarg)
    {
        const string     filename = argv[iarg];
        const PixmapRGBA img4( IMG.loadRGBA(filename,NULL) );
        Domains          seq_doms(img4,seq.num_threads());
        Domains          par_doms(img4,par.num_threads());
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
        channels.split(img4,chan,seq_doms,seq);

        IMG.save( vformat("r%d-seq.png",iarg),rr,toRed,NULL);
        IMG.save( vformat("g%d-seq.png",iarg),gg,toGreen,NULL);
        IMG.save( vformat("b%d-seq.png",iarg),bb,toBlue,NULL);

        chan.ldz();
        channels.split(img4,chan,par_doms,par);
        IMG.save( vformat("r%d-par.png",iarg),rr,toRed,NULL);
        IMG.save( vformat("g%d-par.png",iarg),gg,toGreen,NULL);
        IMG.save( vformat("b%d-par.png",iarg),bb,toBlue,NULL);

        PixmapRGB img3(w,h);
        

    }
}
YOCTO_UNIT_TEST_DONE()

