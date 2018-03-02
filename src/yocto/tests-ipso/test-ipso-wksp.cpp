#include "yocto/ipso/workspace.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/string/conv.hpp"
#include "yocto/ptr/arc.hpp"

using namespace yocto;
using namespace ipso;


YOCTO_UNIT_TEST_IMPL(wksp)
{
#if 0
    __SHOW(workspace<coord1D>);
    __SHOW(workspace<coord2D>);
    __SHOW(workspace<coord3D>);

    
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




    {
        std::cerr << std::endl;
        std::cerr << "In 1D" << std::endl;
        typedef arc_ptr< workspace<coord1D> > wPtr;
        const patch1D       region(1,dims.x);
        coord1D             fallback =0;
        const coord1D       sizes = divider<coord1D>::optimal_for(region,cpus,&fallback);
        std::cerr << "sizes=" << sizes << std::endl;
        const size_t        cores    = __coord_prod(sizes);
        const divide::in1D  full(sizes,region);
        vector<wPtr>        workspaces(cores,as_capacity);
        fields              fvar;
        for(size_t rank=0;rank<cores;++rank)
        {
            std::cerr << "---> rank=" << rank << std::endl;
            wPtr pW(new workspace<coord1D>(full,rank,ng,pbcs.x,32) );
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
        std::cerr << std::endl;
        std::cerr << "In 2D" << std::endl;

        typedef arc_ptr< workspace<coord2D> > wPtr;
        const patch2D       region(coord2D(1,1),dims.xy());
        coord2D             fallback;
        const coord2D       sizes = divider<coord2D>::optimal_for(region,cpus,&fallback);
        std::cerr << "sizes=" << sizes << std::endl;
        const size_t        cores    = __coord_prod(sizes);
        const divide::in2D  full(sizes,region);
        vector<wPtr>        workspaces(cores,as_capacity);
        for(size_t rank=0;rank<cores;++rank)
        {
            std::cerr << "---> rank=" << rank << std::endl;
            wPtr pW(new workspace<coord2D>(full,rank,ng,pbcs.xy(),32) );
            workspaces.push_back(pW);
            pW->create< field2D<int>   >("A");
            pW->create< field2D<short> >("B");
        }

        mimic_exchange(workspaces);

    }

    {
        std::cerr << std::endl;
        std::cerr << "In 3D" << std::endl;

        typedef arc_ptr< workspace<coord3D> > wPtr;
        const patch3D       region(coord3D(1,1,1),dims);
        coord3D             fallback;
        const coord3D       sizes = divider<coord3D>::optimal_for(region,cpus,&fallback);
        std::cerr << "sizes=" << sizes << std::endl;
        const size_t        cores    = __coord_prod(sizes);
        const divide::in3D  full(sizes,region);
        vector<wPtr>        workspaces(cores,as_capacity);
        for(size_t rank=0;rank<cores;++rank)
        {
            std::cerr << "---> rank=" << rank << std::endl;
            wPtr pW(new workspace<coord3D>(full,rank,ng,pbcs,64) );
            workspaces.push_back(pW);
            pW->create< field3D<bool>   >("A");
            pW->create< field3D< point3d<double> > >("B");
        }

        mimic_exchange(workspaces);

    }

#endif
    
}
YOCTO_UNIT_TEST_DONE()

