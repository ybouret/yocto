
#include "yocto/ipso/xbuffer.hpp"
#include "yocto/ipso/partition.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/string/conv.hpp"

using namespace yocto;
using namespace ipso;


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
    const coord1D sizes = partition1D::optimal(cpus,ng,region,PBC, &fallback,NULL);
    std::cerr << "sizes=" << sizes << " | fallback=" << fallback << std::endl;
    const divide::in1D full(sizes,region);
    const partition1D  parts(full,ng,PBC,true);
    size_t count_max = 0;
    for(const domain1D *d=parts.head;d;d=d->next)
    {
        
    }
}


YOCTO_UNIT_TEST_IMPL(xcom)
{
    if(argc<=4)
    {
        throw exception("usage: %s DIMS PBCS GHOSTS CPUS", argv[0]);
    }
    const coord3D dims = __coord_parser::get<coord3D>(argv[1],"dims");
    const coord3D pbcs = __coord_parser::get<coord3D>(argv[1],"pbcs");
    const size_t  ng   = strconv::to_size(argv[3],"ng");
    const size_t  cpus = strconv::to_size(argv[4],"cpus");
    std::cerr << "dims=" << dims << std::endl;
    std::cerr << "pbcs=" << pbcs << std::endl;
    std::cerr << "ng  =" << ng << std::endl;
    std::cerr << "cpus=" << cpus << std::endl;

    test1D<float>(dims,pbcs,cpus,ng);
}
YOCTO_UNIT_TEST_DONE()

