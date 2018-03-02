
#include "yocto/utest/run.hpp"
#include "yocto/ipso/partition.hpp"
#include "yocto/string/conv.hpp"
#include "yocto/string/tokenizer.hpp"
#include "yocto/ipso/field3d.hpp"
#include "yocto/ptr/arc.hpp"
#include "yocto/ipso/subset.hpp"

#include <cstdio>

using namespace yocto;
using namespace ipso;



//static size_t ng = 1;

YOCTO_UNIT_TEST_IMPL(opt)
{
#if 0
    if(argc<=2)
    {
        throw exception("usage: %s DIMS CPUS", argv[0]);
    }
    const coord3D dims = __coord_parser::get<coord3D>(argv[1],"dims");
    const size_t  cpus = strconv::to_size(argv[2],"cpus");
    std::cerr << "dims=" << dims << std::endl;
    std::cerr << "cpus=" << cpus << std::endl;

    if(true)
    {
        const patch1D zone(1,dims.x);
        coord1D fallback = 0;
        coord1D sizes = divider<coord1D>::optimal_for(zone,cpus,&fallback);
        std::cerr << "opt_sizes=" << sizes << ", fallback=" << fallback << std::endl;
    }

    if(true)
    {
        const patch2D zone( coord2D(1,1), dims.xy() );
        coord2D fallback;
        coord2D sizes = divider<coord2D>::optimal_for(zone,cpus,&fallback);
        std::cerr << "opt_sizes=" << sizes << ", fallback=" << fallback << std::endl;
    }

    if(true)
    {
        const patch3D zone( coord3D(1,1,1), dims );
        coord3D fallback;
        coord3D sizes = divider<coord3D>::optimal_for(zone,cpus,&fallback);
        std::cerr << "opt_sizes=" << sizes << ", fallback=" << fallback << std::endl;
    }
#endif

}
YOCTO_UNIT_TEST_DONE()
