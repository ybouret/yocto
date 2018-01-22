
#include "yocto/utest/run.hpp"
#include "yocto/ipso/partition.hpp"
#include "yocto/string/conv.hpp"
#include "yocto/string/tokenizer.hpp"
#include "yocto/ipso/field3d.hpp"
#include "yocto/ptr/arc.hpp"

#include <cstdio>

using namespace yocto;
using namespace ipso;



static size_t ng = 1;

YOCTO_UNIT_TEST_IMPL(opt)
{
    if(argc<=4)
    {
        throw exception("usage: %s DIMS PBCS GHOSTS CPUS", argv[0]);
    }
    const coord3D dims = __coord_parser::get<coord3D>(argv[1],"dims");
    const coord3D pbcs = __coord_parser::get<coord3D>(argv[2],"pbcs");
    ng                 = strconv::to_size(argv[3],"ng");
    const size_t  cpus = strconv::to_size(argv[4],"cpus");
    std::cerr << "dims=" << dims << std::endl;
    std::cerr << "pbcs=" << pbcs << std::endl;
    std::cerr << "ng  =" << ng << std::endl;
    std::cerr << "cpus=" << cpus << std::endl;

    FILE *fp = NULL;
    if(true)
    {
        const patch1D zone(1,dims.x);
        coord1D fallback = 0;
        coord1D sizes = partition<coord1D>::optimal(cpus,ng,zone,pbcs.x,&fallback,fp);
        std::cerr << "opt_sizes=" << sizes << ", fallback=" << fallback << std::endl;
    }


    if(true)
    {
        const patch2D zone( coord2D(1,1), coord2D(dims.x,dims.y) );
        coord2D fallback;
        coord2D sizes = partition<coord2D>::optimal(cpus,ng,zone,coord2D(pbcs.x,pbcs.y),&fallback,fp);
        std::cerr << "opt_sizes=" << sizes << ", fallback=" << fallback << std::endl;
    }

    if(true)
    {
        const patch3D zone( coord3D(1,1,1), coord3D(dims.x,dims.y,dims.z) );
        coord3D fallback;
        coord3D sizes = partition<coord3D>::optimal(cpus,ng,zone,pbcs,&fallback,fp);
        std::cerr << "opt_sizes=" << sizes << ", fallback=" << fallback << std::endl;
    }

}
YOCTO_UNIT_TEST_DONE()

