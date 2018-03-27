#include "yocto/ipso/workspace.hpp"
#include "yocto/ipso/mapping.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/string/conv.hpp"
#include "yocto/ptr/arc.hpp"

using namespace yocto;
using namespace ipso;

template <typename WP>
void mimic_exchange( array<WP> &workspaces )
{
    fields fvar;
    for(size_t i=workspaces.size();i>0;--i)
    {
        WP &pW = workspaces[i];
        fvar.free();
        fvar.append( (*pW)["A"] );
        fvar.append( (*pW)["B"] );
        if(1==i)
        {
            std::cerr << "block_size=" << fvar.block_size() << std::endl;
        }
        pW->allocate_swaps_for(fvar);

        pW->sync_start(fvar);

        pW->sync_query(fvar);
    }

}


#define __SHOW(TYPE) std::cerr << "sizeof(" #TYPE ")=" << sizeof(TYPE) << std::endl
YOCTO_UNIT_TEST_IMPL(wksp)
{
    __SHOW(workspace<coord1D>);
    __SHOW(workspace<coord2D>);
    __SHOW(workspace<coord3D>);

    
    if(argc<=4)
    {
        throw exception("usage: %s DIMS PBCS CPUS LAYERS", argv[0]);
    }
    const coord3D dims   = __coord_parser::get<coord3D>(argv[1],"dims");
    const coord3D pbcs   = __coord_parser::get<coord3D>(argv[2],"pbcs");
    const size_t  cpus   = strconv::to_size(argv[3],"cpus");
    const size_t  layers = strconv::to_size(argv[4],"layers");

    std::cerr << "dims   =" << dims   << std::endl;
    std::cerr << "pbcs   =" << pbcs   << std::endl;
    std::cerr << "cpus   =" << cpus   << std::endl;
    std::cerr << "layers =" << layers << std::endl;



    {
        typedef arc_ptr< workspace<coord1D> > w1p;

        std::cerr << std::endl << "-------- 1D --------" << std::endl;
        const patch1D      region(1,dims.x);
        const coord1D      PBCS(pbcs.x);
        coord1D            fallback=0;
        const coord1D      sizes = mapping<coord1D>::optimal_sizes_for(cpus,region,layers,PBCS,&fallback);
        std::cerr << "sizes=" << sizes << " | fallback=" << fallback << std::endl;
        const divide::in1D full(sizes,region);
        const size_t cores = __coord_prod(sizes);

        vector<w1p>         workspaces(cores,as_capacity);
        fields              fvar;
        for(size_t rank=0;rank<cores;++rank)
        {
            std::cerr << "---> rank=" << rank << std::endl;
            w1p pW(new workspace<coord1D>(full,rank,layers,PBCS) );
            workspaces.push_back(pW);
            field1D<float>  &A = pW->create< field1D<float>  >("A");
            field1D<double> &B = pW->create< field1D<double> >("B");
            if(0==rank)
            {
                fvar.append(A);
                fvar.append(B);
            }
        }

        mimic_exchange(workspaces);

    }


    {
        std::cerr << std::endl << "-------- 2D --------" << std::endl;
        const patch2D      region(coord2D(1,1),dims.xy());
        const coord2D      PBCS(pbcs.xy());
        coord2D            fallback;
        const coord2D sizes = mapping<coord2D>::optimal_sizes_for(cpus,region,layers,PBCS,&fallback);
        std::cerr << "sizes=" << sizes << " | fallback=" << fallback << std::endl;
        const divide::in2D full(sizes,region);
        const size_t cores = __coord_prod(sizes);

        typedef arc_ptr< workspace<coord2D> > w2p;
        vector<w2p>         workspaces(cores,as_capacity);

        for(size_t rank=0;rank<cores;++rank)
        {
            std::cerr << "---> rank=" << rank << std::endl;
            w2p pW(new workspace<coord2D>(full,rank,layers,PBCS) );
            workspaces.push_back(pW);
            pW->create< field2D<float>  >("A");
            pW->create< field2D< point2d<float>  > >("B");
        }

        mimic_exchange(workspaces);
    }


    {
        std::cerr << std::endl << "-------- 3D --------" << std::endl;
        const patch3D      region(coord3D(1,1,1),dims);
        const coord3D      PBCS(pbcs);
        coord3D            fallback;
        const coord3D sizes = mapping<coord3D>::optimal_sizes_for(cpus,region,layers,PBCS,&fallback);
        std::cerr << "sizes=" << sizes << " | fallback=" << fallback << std::endl;
        const divide::in3D full(sizes,region);
        const size_t cores = __coord_prod(sizes);

        typedef arc_ptr< workspace<coord3D> > w3p;
        vector<w3p>         workspaces(cores,as_capacity);

        for(size_t rank=0;rank<cores;++rank)
        {
            std::cerr << "---> rank=" << rank << std::endl;
            w3p pW(new workspace<coord3D>(full,rank,layers,PBCS) );
            workspaces.push_back(pW);
            pW->create< field3D<double>  >("A");
            pW->create< field3D< point3d<double>  > >("B");
        }

        mimic_exchange(workspaces);
    }


    
}
YOCTO_UNIT_TEST_DONE()

