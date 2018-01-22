#include "yocto/ipso/workspace.hpp"
#include "yocto/ipso/partition.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/string/conv.hpp"

using namespace yocto;
using namespace ipso;

#define __SHOW(TYPE) std::cerr << "sizeof(\t" << #TYPE << "\t) = " << sizeof(TYPE) << std::endl


YOCTO_UNIT_TEST_IMPL(wksp)
{
    __SHOW(workspace<coord1D>);
    __SHOW(workspace<coord2D>);
    __SHOW(workspace<coord3D>);

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




    {
        const patch1D       region(1,dims.x);
        coord1D             fallback =0;
        coord1D             sizes    = partition<coord1D>::optimal(cpus,ng,region,pbcs.x,&fallback,NULL);
        const size_t        cores    = __coord_prod(sizes);
        const divide::in1D  full(sizes,region);
        
    }


}
YOCTO_UNIT_TEST_DONE()

