#include "yocto/ipso/workspace.hpp"
#include "yocto/ipso/partition.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/string/conv.hpp"
#include "yocto/ptr/arc.hpp"

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
        typedef arc_ptr< workspace<coord1D> > wPtr;
        const patch1D       region(1,dims.x);
        coord1D             fallback =0;
        coord1D             sizes    = partition<coord1D>::optimal(cpus,ng,region,pbcs.x,&fallback,NULL);
        const size_t        cores    = __coord_prod(sizes);
        const divide::in1D  full(sizes,region);
        vector<wPtr> workspaces(cores,as_capacity);
        for(size_t rank=0;rank<cores;++rank)
        {
            wPtr pW(new workspace<coord1D>(full,rank,ng,pbcs.x) );
            workspaces.push_back(pW);
        }
    }


}
YOCTO_UNIT_TEST_DONE()

