#include "yocto/mpi/split.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;

namespace
{
    template <typename T>
    static inline
    void test_split( const point2d<T> &layout, const size_t sz_max)
    {
        for(size_t sz=1;sz<=sz_max;++sz)
        {
            const point2d<T> map2d = mpi_split::compute_sizes(sz,layout);
            std::cerr << "map2d=" << map2d << std::endl;
        }
    }
}

YOCTO_UNIT_TEST_IMPL(split)
{
    std::cerr << "Testing Split..." << std::endl;
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

    std::cerr << "Testing Auto Split" << std::endl;

    const size_t sz_max = 9;
    {
        point2d<int>  l2d_sq(100,100);
        test_split(l2d_sq,sz_max);
    }

    {
        point2d<long> l2d_xmajor(100,80);
        test_split(l2d_xmajor,sz_max);
    }

    {
        point2d<long> l2d_ymajor(80,100);
        test_split(l2d_ymajor,sz_max);
    }



}
YOCTO_UNIT_TEST_DONE()
