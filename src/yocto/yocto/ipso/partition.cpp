#include "yocto/ipso/partition.hpp"
#include "yocto/code/utils.hpp"

namespace yocto
{
    namespace ipso
    {
        template <>
        coord1D partition<coord1D>::optimal(const size_t        max_cpus,
                                            const size_t        ,
                                            const patch<coord1D> &zone,
                                            const coord1D         )
        {
            return min_of<coord1D>(zone.width,max_of<coord1D>(max_cpus,1));
        }
    }
}


namespace yocto
{
    namespace ipso
    {
        template <>
        coord2D partition<coord2D>::optimal(const size_t          max_cpus,
                                            const size_t          num_ghosts,
                                            const patch<coord2D> &zone,
                                            const coord2D         pbcs)
        {
            const coord1D n  = max_of<coord1D>(1,max_cpus);
            const coord1D nx = min_of(n,zone.width.x);
            const coord1D ny = min_of(n,zone.width.y);

            std::cerr << "building list of partitions..." << std::endl;
            coord2D           sizes;
            partition2D::list plist;
            for(sizes.x=1;sizes.x<=nx;++sizes.x)
            {
                for(sizes.y=1;sizes.y<=ny;++sizes.y)
                {
                    if( sizes.__prod() > n ) continue;
                    std::cerr << "accepting sizes=" << sizes << std::endl;
                    divide::in2D D(sizes,zone);
                    partition2D *p = new partition(D,num_ghosts,pbcs,false);
                    plist.push_back(p);
                }
            }
            std::cerr << "#partitions=" << plist.size << std::endl;

            return coord2D(1,n);
        }
    }
}
