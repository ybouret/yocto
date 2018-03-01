
#include "yocto/ipso/subset.hpp"

namespace yocto
{
    namespace ipso
    {
        template <>
        mapping<coord1D>:: mapping(const size_t          cpus,
                                   const patch<coord1D> &full,
                                   const size_t          layers,
                                   const coord1D         pbcs)
        {
            const coord1D max_cpus = clamp<size_t>(1,cpus,full.width);
            for(coord1D n=1;n<=max_cpus;++n)
            {
                const divide::in1D D(n,full);
                push_back(new subsets<coord1D>(D,layers,pbcs,false));
            }
        }

    }
}

namespace yocto
{
    namespace ipso
    {
        template <>
        mapping<coord2D>:: mapping(const size_t          cpus,
                                   const patch<coord2D> &full,
                                   const size_t          layers,
                                   const coord2D         pbcs)
        {
            const coord1D nc = max_of<size_t>(1,cpus);
            const coord1D nx = clamp<coord1D>(1,nc,full.width.x);
            const coord1D ny = clamp<coord1D>(1,nc,full.width.y);

            coord2D sizes;
            for(sizes.x=1;sizes.x<=nx;++sizes.x)
            {
                for(sizes.y=1;sizes.y<=ny;++sizes.y)
                {
                    if( sizes.__prod() <= nc )
                    {
                        const divide::in2D D(sizes,full);
                        push_back(new subsets<coord2D>(D,layers,pbcs,false));
                    }
                }
            }

        }

    }
}

namespace yocto
{
    namespace ipso
    {
        template <>
        mapping<coord3D>:: mapping(const size_t          cpus,
                                   const patch<coord3D> &full,
                                   const size_t          layers,
                                   const coord3D         pbcs)
        {
            const coord1D nc = max_of<size_t>(1,cpus);
            const coord1D nx = clamp<coord1D>(1,nc,full.width.x);
            const coord1D ny = clamp<coord1D>(1,nc,full.width.y);
            const coord1D nz = clamp<coord1D>(1,nc,full.width.z);

            coord3D sizes;
            for(sizes.x=1;sizes.x<=nx;++sizes.x)
            {
                for(sizes.y=1;sizes.y<=ny;++sizes.y)
                {
                    for(sizes.z=1;sizes.z<=nz;++sizes.z)
                    {
                        if( sizes.__prod() <= nc )
                        {
                            const divide::in3D D(sizes,full);
                            push_back(new subsets<coord3D>(D,layers,pbcs,false));
                        }
                    }
                }
            }

        }

    }
}
