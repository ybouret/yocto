
#include "yocto/ink/ops/histogram.hpp"
#include "yocto/ink/ops/edm.hpp"
#include "yocto/ink/color/conv.hpp"
#include "yocto/ink/image.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/ink/color/ramp/cold_to_very_hot.hpp"
#include "yocto/ink/ops/filter.hpp"


using namespace yocto;
using namespace Ink;


static inline void build_dmap(const unit_t W, const unit_t H)
{
    std::cerr << "Building dmap " << W << "x" << H << std::endl;
    map<unit_t,size_t> dmap(W*H,as_capacity);
    for(unit_t j=1;j<H;++j)
    {
        const unit_t j2 = j*j;
        for(unit_t i=1;i<W;++i)
        {
            const unit_t d2 = j2 + i*i;
            size_t *pCount = dmap.search(d2);
            if( pCount )
            {
                ++(*pCount);
            }
            else
            {
                (void) dmap.insert(d2,1);
            }
        }
    }
    dmap.sort_keys_by( __compare<unit_t> );
    std::cerr << "#distances=" << dmap.size() << "/" << W*H << std::endl;
    size_t count = 0;
    {
        ios::wcstream fp("dmap.dat");
        for( map<unit_t,size_t>::iterator i=dmap.begin(); i != dmap.end(); ++i)
        {
            const unit_t d2 = i->key;
            const size_t n  = *i;
            const float d = sqrtf( float(d2) );
            fp("%.15g %u\n", d, unsigned(n) );
            count += n;
        }
    }
    std::cerr << "#probes   =" << count << std::endl;
}

YOCTO_UNIT_TEST_IMPL(edm)
{
    YOCTO_IMG();

    Engine::SharedServer seqSrv( new threading::seq_server() );
    Engine::SharedServer parSrv( new threading::par_server() );

    Histogram H;

    build_dmap(1024,512);
    build_dmap(512,1024);


    if(argc>1)
    {
        const PixmapRGB pxm( IMG.loadRGB(argv[1],NULL) );
        IMG.save(pxm,"img.png",NULL);
        Engine    par(pxm,parSrv);
        Histogram H;
        PixmapRGB fg(pxm.w,pxm.h);
        H.foreground(fg,pxm, Convert::RGB2U, par,true);
        IMG.save(fg,"fg.png",NULL);

        EDM edm(pxm.w,pxm.h);
        
        edm.compute(fg,par);
        ramp_cold_to_very_hot cr(0,edm.ed_max);
        IMG.save("edm.png", edm, cr, NULL);

        

    }

}
YOCTO_UNIT_TEST_DONE()




