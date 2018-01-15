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
                                            const coord1D         )
        {
            std::cerr << "1D optimized partition..." << std::endl;
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
            //__________________________________________________________________
            //
            // prepare valid cuts
            //__________________________________________________________________
            const coord1D n  = max_of<coord1D>(1,max_cpus);
            const coord1D nx = min_of(n,zone.width.x);
            const coord1D ny = min_of(n,zone.width.y);
            bool          match =  false;
            std::cerr << "2D building list of partitions for <= " << n << " cpus" << std::endl;
            partition2D::list plist;
            {
                coord2D           sizes;
                for(sizes.x=1;sizes.x<=nx;++sizes.x)
                {
                    for(sizes.y=1;sizes.y<=ny;++sizes.y)
                    {
                        const coord1D np = sizes.__prod();
                        if(np>n)  continue;
                        if(np==n) match = true;

                        divide::in2D D(sizes,zone);                                 // create a divider
                        partition2D *p = new partition(D,num_ghosts,pbcs,false);    // all the domains in the partition, no ghost coodinates
                        plist.push_back(p);                                         // append to this
                        assert(sizes.__prod()==coord1D(p->size));
                    }
                }
            }
            if(false&&match)
            {
                partition2D::list tmp;
                while(plist.size>0)
                {
                    partition2D *p = plist.pop_front();
                    if(1==p->size||2==p->size||n==p->size)
                    {
                        tmp.push_back(p);
                    }
                    else
                    {
                        delete p;
                    }
                }
                plist.swap_with(tmp);
            }


            //__________________________________________________________________
            //
            // rank by cores/splitting: first is one core, the slowest
            //__________________________________________________________________
            core::merging<partition>::sort(plist,partition<coord2D>::compare_by_cores, NULL);

            std::cerr << "#partitions=" << plist.size << std::endl;
            for(const partition *p = plist.head;p;p=p->next)
            {
                std::cerr << "accepting sizes=" << p->sizes << ", #cpu=" << p->size << std::endl;
                for(const domain2D *d = p->head; d; d=d->next)
                {
                    std::cerr << "\t" << d->ranks << " : " << d->load << " | " << d->inner << std::endl;
                }
            }

            //__________________________________________________________________
            //
            // then use algorithm
            //__________________________________________________________________
            return compute_optimal_from(plist);
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
                                            const coord3D         pbcs)
        {
            //__________________________________________________________________
            //
            // prepare valid cuts
            //__________________________________________________________________
            const coord1D n  = max_of<coord1D>(1,max_cpus);
            const coord1D nx = min_of(n,zone.width.x);
            const coord1D ny = min_of(n,zone.width.y);
            const coord1D nz = min_of(n,zone.width.z);

            std::cerr << "3D building list of partitions for <= " << n << " cpus" << std::endl;
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
            // rank by cores/splitting: first is one core, the slowest
            //__________________________________________________________________
            core::merging<partition>::sort(plist,partition<coord3D>::compare_by_cores, NULL);

            std::cerr << "#partitions=" << plist.size << std::endl;
            for(const partition *p = plist.head;p;p=p->next)
            {
                std::cerr << "accepting sizes=" << p->sizes << ", #cpu=" << p->size << std::endl;
                for(const domain3D *d = p->head; d; d=d->next)
                {
                    std::cerr << "\t" << d->ranks << " : " << d->load << std::endl;
                }
            }


            //__________________________________________________________________
            //
            // then use algorithm
            //__________________________________________________________________
            return compute_optimal_from(plist);
        }
        
    }
}
