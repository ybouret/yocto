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
            // 2D
            {
                const int size = sx * sy;
                const point2d<long> sizes(sx,sy);
                std::cerr << "sizes=" << sizes << ", size=" << size << std::endl;
                for(int rank=0;rank<size;++rank)
                {
                    const point2d<long> ranks = mpi_split::local_ranks(rank,sizes);
                    const int           r     = mpi_split::get_rank_of(ranks,sizes);
                    std::cerr << "\trank=" << rank << " => " << ranks << std::endl;
                    if(r!=rank) throw exception("ranks 2D mismatch");
                }
            }
            
            // 3D
            for(int sz=1;sz<=smax;++sz)
            {
                const int size=sx*sy*sz;
                const point3d<int> sizes(sx,sy,sz);
                std::cerr << "sizes=" << sizes << ", size=" << size << std::endl;
                for(int rank=0;rank<size;++rank)
                {
                    const point3d<int> ranks = mpi_split::local_ranks(rank,sizes);
                    const int          r     = mpi_split::get_rank_of(ranks,sizes);
                    std::cerr << "\trank=" << rank << " => " << ranks << std::endl;
                    if(r!=rank) throw exception("ranks 3D mismatch");
                }
            }
        }
    }
    
}
YOCTO_UNIT_TEST_DONE()
