#include "yocto/mpi/split.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;

YOCTO_UNIT_TEST_IMPL(split)
{
    const int smax = 4;
    for(int sx=1;sx<=smax;++sx)
    {
        for(int sy=1;sy<=smax;++sy)
        {
            const int size = sx * sy;
            const point2d<long> sizes(sx,sy);
            std::cerr << "sizes=" << sizes << ", size=" << size << std::endl;
            for(int rank=0;rank<size;++rank)
            {
                const point2d<long> ranks = mpi_split::local_ranks(rank,sizes);
                std::cerr << "\trank=" << rank << " => " << ranks << std::endl;
                const int           r     = mpi_split::get_rank_of(ranks,sizes);
                if(r!=rank) throw exception("ranks mismatch");
            }
        }
    }
    
}
YOCTO_UNIT_TEST_DONE()
