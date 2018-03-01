#include "yocto/ipso/subset.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/string/conv.hpp"

using namespace yocto;
using namespace ipso;

template <typename COORD>
static inline void display( const mapping<COORD> &maps )
{
    std::cerr << "#subsets=" << maps.size << std::endl;
    for( const subsets<COORD> *subs = maps.head; subs; subs=subs->next )
    {
        std::cerr << "sizes=" << subs->sizes << std::endl;
#if 0
        for(const subset<COORD> *sub = subs->head; sub; sub=sub->next)
        {
            std::cerr << "\t" << sub->ranks << "\t: score=" << sub->score << std::endl;
        }
#endif
        std::cerr << "\t|_highest score=" << subs->score << std::endl;
    }
}


YOCTO_UNIT_TEST_IMPL(subopt)
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
        mapping<coord1D>   maps(cpus,region,layers,PBCS);
        display(maps);
    }


    if(true)
    {
        std::cerr << std::endl << "-------- 2D --------" << std::endl;
        const patch2D          region(coord2D(1,1),dims.xy());
        const coord2D          PBCS(pbcs.xy());
        mapping<coord2D>       maps(cpus,region,layers,PBCS);
        display(maps);
    }

    if(false)
    {
        std::cerr << std::endl << "-------- 3D --------" << std::endl;
        const patch3D      region(coord3D(1,1,1),dims);
        const coord3D      PBCS(pbcs);
        mapping<coord3D>   maps(cpus,region,layers,PBCS);
        display(maps);
    }


}
YOCTO_UNIT_TEST_DONE()


