
#include "yocto/ipso/mapping.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/string/conv.hpp"
#include "yocto/ipso/format/vtk.hpp"
#include "yocto/ios/ocstream.hpp"

using namespace yocto;
using namespace ipso;

YOCTO_UNIT_TEST_IMPL(onesub)
{
    if(argc<=4)
    {
        throw exception("usage: %s DIMS PBCS CPUS LAYERS", argv[0]);
    }

    const coord3D dims   = __coord_parser::get<coord3D>(argv[1],"dims");
    const coord3D pbcs   = __coord_parser::get<coord3D>(argv[2],"pbcs");
    const coord3D cpus   = __coord_parser::get<coord3D>(argv[3],"cpus");
    const size_t  layers = strconv::to_size(argv[4],"layers");

    std::cerr << "dims   = " << dims   << std::endl;
    std::cerr << "pbcs   = " << pbcs   << std::endl;
    std::cerr << "cpus   = " << cpus   << std::endl;
    std::cerr << "layers = " << layers << std::endl;

    //const size_t ncpu = __coord_prod(cpus);

    if(true)
    {
        const patch2D      region(coord2D(1,1),dims.xy());
        const coord2D      PBCS(pbcs.xy());
        const coord2D      sizes(cpus.xy());
        const divide::in2D full(sizes,region);
        subsets<coord2D>   subs(full,layers,PBCS,true);

    }

}
YOCTO_UNIT_TEST_DONE()

