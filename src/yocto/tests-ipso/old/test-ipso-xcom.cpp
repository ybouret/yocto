
#include "yocto/ipso/xbuffer.hpp"
#include "yocto/ipso/partition.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/string/conv.hpp"
#include "yocto/ipso/field3d.hpp"

using namespace yocto;
using namespace ipso;

#if 0
template <typename T>
static inline
void test1D(const coord3D dims,
            const coord3D pbcs,
            const coord1D cpus,
            const coord1D ng)
{
    std::cerr << "######## 1D/cpus=" << cpus << std::endl;
    const patch1D      region(1,dims.x);
    const coord1D      PBC(pbcs.x);
    coord1D            fallback=0;
    const coord1D sizes = divider<coord1D>::optimal_for(region,cpus,&fallback);
    std::cerr << "sizes=" << sizes << " | fallback=" << fallback << std::endl;
    const divide::in1D full(sizes,region);
    const partition1D  parts(full,ng,PBC,true);
    for(const domain1D *d=parts.head;d;d=d->next)
    {
        std::cerr << "domain1D " << d->ranks << " : inner=" << d->inner << " | outer=" << d->outer << std::endl;
        std::cerr << "\tasyncs=" << d->asyncs << std::endl;
        const size_t nm = __coord_max(d->asyncs);
        std::cerr << "\txbuff with " << nm << std::endl;
        field1D<T> F("f1d",d->outer);
        exchange_buffer xbuff(nm*sizeof(T));

        for(size_t dim=0;dim<1;++dim)
        {
            for(const ghosts *G = d->async[dim].head;G;G=G->next)
            {
                xbuff.reset();
                F.store(*G,xbuff);
                F.query(*G,xbuff);
            }
        }
    }
}

template <typename T>
static inline
void test2D(const coord3D dims,
            const coord3D pbcs,
            const coord1D cpus,
            const coord1D ng)
{
    std::cerr << "######## 2D/cpus=" << cpus << std::endl;
    const patch2D      region( coord2D(1,1),coord2D(dims.x,dims.y));
    const coord2D      PBC(pbcs.x,pbcs.y);
    coord2D            fallback;
    const coord2D      sizes = divider<coord2D>::optimal_for(region,cpus,&fallback);

    std::cerr << "sizes=" << sizes << " | fallback=" << fallback << std::endl;
    const divide::in2D full(sizes,region);
    const partition2D  parts(full,ng,PBC,true);
    for(const domain2D *d=parts.head;d;d=d->next)
    {
        std::cerr << "domain2D " << d->ranks << " : inner=" << d->inner << " | outer=" << d->outer << std::endl;
        std::cerr << "\tasyncs=" << d->asyncs << std::endl;
        const size_t nm = __coord_max(d->asyncs);
        std::cerr << "\txbuff with " << nm << std::endl;
        field2D<T> F("f2d",d->outer);
        exchange_buffer xbuff(nm*sizeof(T));

        for(size_t dim=0;dim<1;++dim)
        {
            for(const ghosts *G = d->async[dim].head;G;G=G->next)
            {
                xbuff.reset();
                F.store(*G,xbuff);
                F.query(*G,xbuff);
            }
        }
    }
}

template <typename T>
static inline
void test3D(const coord3D dims,
            const coord3D pbcs,
            const coord1D cpus,
            const coord1D ng)
{
    std::cerr << "######## 3D/cpus=" << cpus << std::endl;
    const patch3D      region( coord3D(1,1,1),dims);
    coord3D            fallback;
    const coord3D      sizes = divider<coord3D>::optimal_for(region,cpus,&fallback);
    std::cerr << "sizes=" << sizes << " | fallback=" << fallback << std::endl;
    const divide::in3D full(sizes,region);
    const partition3D  parts(full,ng,pbcs,true);
    for(const domain3D *d=parts.head;d;d=d->next)
    {
        std::cerr << "domain3D " << d->ranks << " : inner=" << d->inner << " | outer=" << d->outer << std::endl;
        std::cerr << "\tasyncs=" << d->asyncs << std::endl;
        const size_t nm = __coord_max(d->asyncs);
        std::cerr << "\txbuff with " << nm << std::endl;
        field3D<T> F("f3d",d->outer);
        exchange_buffer xbuff(nm*sizeof(T));

        for(size_t dim=0;dim<1;++dim)
        {
            for(const ghosts *G = d->async[dim].head;G;G=G->next)
            {
                xbuff.reset();
                F.store(*G,xbuff);
                F.query(*G,xbuff);
            }
        }
    }
}



#endif

#define __SHOW(TYPE) std::cerr << "sizeof(\t" << #TYPE << "\t) = " << sizeof(TYPE) << std::endl

YOCTO_UNIT_TEST_IMPL(xcom)
{
#if 0
    if(argc<=4)
    {
        throw exception("usage: %s DIMS PBCS GHOSTS CPUS", argv[0]);
    }
    const coord3D dims = __coord_parser::get<coord3D>(argv[1],"dims");
    const coord3D pbcs = __coord_parser::get<coord3D>(argv[2],"pbcs");
    const size_t  ng   = strconv::to_size(argv[3],"ng");
    const size_t  cpus = strconv::to_size(argv[4],"cpus");
    std::cerr << "dims=" << dims << std::endl;
    std::cerr << "pbcs=" << pbcs << std::endl;
    std::cerr << "ng  =" << ng << std::endl;
    std::cerr << "cpus=" << cpus << std::endl;

    test1D<float>(dims,pbcs,cpus,ng);
    test2D<double>(dims,pbcs,cpus,ng);
    test3D<uint16_t>(dims,pbcs,cpus,ng);

    __SHOW(domain1D);
    __SHOW(domain2D);
    __SHOW(domain3D);
#endif
    
}
YOCTO_UNIT_TEST_DONE()
