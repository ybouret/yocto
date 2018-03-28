#include "yocto/ipso/mapping.hpp"
#include "yocto/string/conv.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ipso/mesh/rectilinear.hpp"

using namespace yocto;
using namespace ipso;


YOCTO_UNIT_TEST_IMPL(mesh)
{
    if(argc<=4)
    {
        throw exception("usage: %s DIMS PBCS CPUS LAYERS", argv[0]);
    }
    const coord3D dims   = __coord_parser::get<coord3D>(argv[1],"dims");
    const coord3D pbcs   = __coord_parser::get<coord3D>(argv[2],"pbcs");
    const size_t  cpus   = strconv::to_size(argv[3],"cpus");
    const size_t  layers = strconv::to_size(argv[4],"layers");
    
    std::cerr << "dims   = " << dims   << std::endl;
    std::cerr << "pbcs   = " << pbcs   << std::endl;
    std::cerr << "cpus   = " << cpus   << std::endl;
    std::cerr << "layers = " << layers << std::endl;
    
    if(true)
    {
        std::cerr << std::endl << "-------- 1D --------" << std::endl;
        const patch1D      region(1,dims.x);
        const coord1D      PBCS(pbcs.x);
        coord1D            fallback=0;
        const coord1D sizes = mapping<coord1D>::optimal_sizes_for(cpus,region,layers,PBCS,&fallback);
        std::cerr << "sizes=" << sizes << " | fallback=" << fallback << std::endl;
        const divide::in1D full(sizes,region);
        subsets<coord1D>   subs(full,layers,PBCS,true);
        axis_names         names("x");
        for( subset<coord1D> *sub = subs.head; sub; sub=sub->next )
        {
            rectilinear_mesh<float,1> rmesh(names,*sub);
        }
        
    }
    
}
YOCTO_UNIT_TEST_DONE()

