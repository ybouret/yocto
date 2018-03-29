#include "yocto/ipso/mapping.hpp"
#include "yocto/string/conv.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ipso/mesh/rectilinear.hpp"
#include "yocto/ipso/mesh/curvilinear.hpp"

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
    
    const patch1D      region1D(1,dims.x);
    const coord1D      PBCS1D(pbcs.x);
    coord1D            fallback1D=0;
    const coord1D      sizes1D = mapping<coord1D>::optimal_sizes_for(cpus,region1D,layers,PBCS1D,&fallback1D);
    const divide::in1D full1D(sizes1D,region1D);
    subsets<coord1D>   subs1D(full1D,layers,PBCS1D,true);
    
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
        const box<float,1> b(0,1);
        for( subset<coord1D> *sub = subs.head; sub; sub=sub->next )
        {
            rectilinear_mesh<float,1>  rmesh(names,*sub);
            rmesh.map_regular(b,full);
            curvilinear_mesh<float,1>  cmesh(names,*sub);
            cmesh.map_regular(b,full);
        }
        
    }
    
    if(true)
    {
        
        std::cerr << std::endl << "-------- 2D --------" << std::endl;
        const patch2D      region(coord2D(1,1),dims.xy());
        const coord2D      PBCS(pbcs.xy());
        coord2D            fallback;
        const coord2D sizes = mapping<coord2D>::optimal_sizes_for(cpus,region,layers,PBCS,&fallback);
        std::cerr << "sizes=" << sizes << " | fallback=" << fallback << std::endl;
        const divide::in2D full(sizes,region);
        subsets<coord2D>   subs(full,layers,PBCS,true);
        
        axis_names         names("x,y");
        const box<float,2> b( point2d<float>(0,0), point2d<float>(1,1) );

        for( subset<coord2D> *sub = subs.head; sub; sub=sub->next )
        {
            rectilinear_mesh<float,2>  rmesh(names,*sub);
            rmesh.map_regular(b,full);
            curvilinear_mesh<float,2> cmesh(names,*sub);
            cmesh.map_regular(b,full);
        }
    }
    
    if(true)
    {
        std::cerr << std::endl << "-------- 3D --------" << std::endl;
        const patch3D      region(coord3D(1,1,1),dims);
        const coord3D      PBCS(pbcs);
        coord3D            fallback;
        const coord3D sizes = mapping<coord3D>::optimal_sizes_for(cpus,region,layers,PBCS,&fallback);
        std::cerr << "sizes=" << sizes << " | fallback=" << fallback << std::endl;
        const divide::in3D full(sizes,region);
        subsets<coord3D>   subs(full,layers,PBCS,true);
        
        axis_names         names("x,y,z");
        const box<float,3> b( point3d<float>(0,0,0), point3d<float>(1,1,1) );
        for( subset<coord3D> *sub = subs.head; sub; sub=sub->next )
        {
            rectilinear_mesh<float,3>  rmesh(names,*sub);
            rmesh.map_regular(b,full);
            curvilinear_mesh<float,3> cmesh(names,*sub);
            cmesh.map_regular(b,full);
        }
        
    }
    
    
    
}
YOCTO_UNIT_TEST_DONE()

