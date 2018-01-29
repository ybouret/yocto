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


        channels.split(img4,chan,seq_doms,seq);
        channels.split(img4,chan,par_doms,par);
    }
}
YOCTO_UNIT_TEST_DONE()

