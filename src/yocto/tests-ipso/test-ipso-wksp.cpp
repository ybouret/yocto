#include "yocto/ipso/workspace.hpp"
#include "yocto/ipso/partition.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/string/conv.hpp"
#include "yocto/ptr/arc.hpp"

using namespace yocto;
using namespace ipso;

#define __SHOW(TYPE) std::cerr << "sizeof(\t" << #TYPE << "\t)\t=\t" << sizeof(TYPE) << std::endl

template <typename COORD>
static inline
void mimic_exchange( array<  arc_ptr<workspace<COORD> > > &workspaces)
{
    const size_t cores = workspaces.size();
    std::cerr << "---> prepare sync..." << std::endl;
    for(size_t rank=0;rank<cores;++rank)
    {
        workspace<COORD> &W = *workspaces[rank+1];
        W.sync_store_begin();
        W.sync_store( W.fields["A"] );
        W.sync_store( W.fields["B"] );

    }
    for(size_t rank=0;rank<cores;++rank)
    {
        workspace<COORD> &W = *workspaces[rank+1];
        W.sync_store_end();
    }
    
    std::cerr << "---> do something with data" << std::endl;


    std::cerr << "---> finalize sync" << std::endl;
    for(size_t rank=0;rank<cores;++rank)
    {
        workspace<COORD> &W = *workspaces[rank+1];
        W.sync_query( W.fields["A"] );
        W.sync_query( W.fields["B"] );
    }
    std::cerr << "---> done" << std::endl;
}

YOCTO_UNIT_TEST_IMPL(wksp)
{
    __SHOW(workspace<coord1D>);
    __SHOW(workspace<coord2D>);
    __SHOW(workspace<coord3D>);
    __SHOW(exchange_buffer);
    __SHOW(exchange_buffers);
    
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
        coord1D             sizes    = partition<coord1D>::optimal(cpus,ng,region,pbcs.x,&fallback,NULL);
        std::cerr << "sizes=" << sizes << std::endl;
        const size_t        cores    = __coord_prod(sizes);
        const divide::in1D  full(sizes,region);
        vector<wPtr>        workspaces(cores,as_capacity);
        for(size_t rank=0;rank<cores;++rank)
        {
            std::cerr << "---> rank=" << rank << std::endl;
            wPtr pW(new workspace<coord1D>(full,rank,ng,pbcs.x,32) );
            workspaces.push_back(pW);
            pW->create< field1D<float>  >("A");
            pW->create< field1D<double> >("B");
        }

        mimic_exchange(workspaces);

    }

    {
        std::cerr << std::endl;
        std::cerr << "In 2D" << std::endl;

        typedef arc_ptr< workspace<coord2D> > wPtr;
        const patch2D       region(coord2D(1,1),dims.xy());
        coord2D             fallback;
        coord2D             sizes    = partition<coord2D>::optimal(cpus,ng,region,pbcs.xy(),&fallback,NULL);
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
        coord3D             sizes    = partition<coord3D>::optimal(cpus,ng,region,pbcs,&fallback,NULL);
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


}
YOCTO_UNIT_TEST_DONE()

