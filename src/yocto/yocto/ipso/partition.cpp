#include "yocto/ipso/partition.hpp"
#include "yocto/code/utils.hpp"
#include "yocto/sort/merge.hpp"

namespace yocto
{
    namespace ipso
    {
        template <>
        coord1D partition<coord1D>::optimal(const size_t        max_cpus,
                                            const size_t        ,
                                            const patch<coord1D> &zone,
                                            const coord1D         ,
                                            coord1D              *fallback,
                                            FILE                 *fp)
        {
            if(fp)
            {
                fprintf(fp,"1D optimized partition\n");
            }
            const coord1D ans = min_of<coord1D>(zone.width,max_of<coord1D>(max_cpus,1));
            if(fallback)
            {
                *fallback = ans;
            }
            return ans;
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
                                            const coord2D         pbcs,
                                            coord2D              *fallback,
                                            FILE                 *fp)
        {
            //__________________________________________________________________
            //
            // prepare valid cuts
            //__________________________________________________________________
            const coord1D n  = max_of<coord1D>(1,max_cpus);
            const coord1D nx = min_of(n,zone.width.x);
            const coord1D ny = min_of(n,zone.width.y);
            if(fp)
            {
                fprintf(fp,"2D building list of partitions for #cpus=%u\n", unsigned(n));
            }
            partition2D::list plist;
            {
                coord2D           sizes;
                for(sizes.x=1;sizes.x<=nx;++sizes.x)
                {
                    for(sizes.y=1;sizes.y<=ny;++sizes.y)
                    {
                        const coord1D np = sizes.__prod();
                        if(np>n)  continue;

                        divide::in2D D(sizes,zone);                                 // create a divider
                        partition2D *p = new partition(D,num_ghosts,pbcs,false);    // all the domains in the partition, no ghost coodinates
                        plist.push_back(p);                                         // append to this
                        assert(sizes.__prod()==coord1D(p->size));
                    }
                }
            }

            
            //__________________________________________________________________
            //
            // then use algorithm
            //__________________________________________________________________
            return compute_optimal_from(plist,fallback,fp);
        }
    }
}

namespace yocto
{
    namespace ipso
    {
        template <>
        coord3D partition<coord3D>::optimal(const size_t          max_cpus,
                                            const size_t          num_ghosts,
                                            const patch<coord3D> &zone,
                                            const coord3D         pbcs,
                                            coord3D              *fallback,
                                            FILE                 *fp)
        {
            //__________________________________________________________________
            //
            // prepare valid cuts
            //__________________________________________________________________
            const coord1D n  = max_of<coord1D>(1,max_cpus);
            const coord1D nx = min_of(n,zone.width.x);
            const coord1D ny = min_of(n,zone.width.y);
            const coord1D nz = min_of(n,zone.width.z);

           if(fp)
           {
               fprintf(fp,"3D building list of partitions for #cpus=%u\n", unsigned(n));
           }

            partition3D::list plist;
            {
                coord3D           sizes;
                for(sizes.x=1;sizes.x<=nx;++sizes.x)
                {
                    for(sizes.y=1;sizes.y<=ny;++sizes.y)
                    {
                        for(sizes.z=1;sizes.z<=nz;++sizes.z)
                        {
                            const coord1D np = sizes.__prod();
                            if(np>n) continue;
                            divide::in3D D(sizes,zone);                                 // create a divider
                            partition3D *p = new partition(D,num_ghosts,pbcs,false);    // all the domains in the partition
                            plist.push_back(p);
                            assert(sizes.__prod()==coord1D(p->size));
                        }
                    }
                }
            }

            
            //__________________________________________________________________
            //
            // then use algorithm
            //__________________________________________________________________
            return compute_optimal_from(plist,fallback,fp);
        }
        
    }
}
