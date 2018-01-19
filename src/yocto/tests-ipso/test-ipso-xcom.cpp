
#include "yocto/ipso/xbuffer.hpp"
#include "yocto/ipso/partition.hpp"
#include "yocto/utest/run.hpp"


using namespace yocto;
using namespace ipso;

static size_t ng = 1;

static inline
void test1D( const coord3D dims, const coord3D pbcs, const coord1D cpus)
{
    std::cerr << "######## 1D/cpus=" << cpus << std::endl;
    const patch1D      region(1,dims.x);
    const coord1D      PBC(pbcs.x);
    coord1D       fallback=0;
    const coord1D sizes = partition1D::optimal(cpus,ng,region,PBC, &fallback,NULL);
    std::cerr << "sizes=" << sizes << " | fallback=" << fallback << std::endl;
    const divide::in1D full(sizes,region);
    const partition1D  parts(full,ng,PBC,true);
}


YOCTO_UNIT_TEST_IMPL(xcom)
{
    
}
YOCTO_UNIT_TEST_DONE()

