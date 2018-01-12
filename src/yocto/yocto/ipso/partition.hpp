#ifndef YOCTO_IPSO_PARTITION_INCLUDED
#define YOCTO_IPSO_PARTITION_INCLUDED 1

#include "yocto/ipso/domain.hpp"
#include "yocto/sequence/addr-list.hpp"

namespace yocto
{
    namespace ipso
    {




        //! a partition is a list of domains
        template <typename COORD>
        class partition : public object, public domain<COORD>::list
        {
        public:
            typedef domain<COORD>                domain_type;
            typedef typename domain_type::list   domains;
            typedef core::list_of_cpp<partition> list;
            typedef addr_list<partition>         meta_list;
            typedef addr_node<partition>         meta_node;

            const COORD sizes; //!< keep track of global sizes
            partition  *next;
            partition  *prev;

            explicit partition(const divider<COORD>  &full,
                               const size_t           ng,
                               const COORD            pbcs,
                               const bool             build=false) :
            domains(),
            sizes(full.sizes),
            next(0),
            prev(0)
            {
                for(size_t rank=0;rank<full.size;++rank)
                {
                    this->push_back( new domain_type(full,rank,ng,pbcs,build) );
                }
            }

            virtual ~partition() throw()
            {
            }

            static COORD optimal(const size_t        max_cpus,
                                 const size_t        num_ghosts,
                                 const patch<COORD> &zone,
                                 const COORD         pbcs);

            //! for 2D and 3D, rank by supposedly fastest partition
            static inline
            int compare_by_cores(const partition *lhs, const partition *rhs, void *) throw()
            {
                const size_t nl = lhs->size;
                const size_t nr = rhs->size;
                if(nl<nr)
                {
                    return -1;
                }
                else
                {
                    if(nr<nl)
                    {
                        return 1;
                    }
                    else
                    {
                        assert(nl==nr);
                        return -(COORD::lexicompare(lhs->sizes,rhs->sizes));
                    }
                }
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(partition);

            static inline
            COORD compute_optimal_from( list &plist )
            {
                partition     *sequential  = plist.head; assert(1==sequential->size);
                const metrics &seq_load    = sequential->head->load;
                std::cerr << "#sequential=" << seq_load << std::endl;
                if( plist.size > 1 )
                {
                    //__________________________________________________________
                    //
                    // take the slowest parallel partitions with 2 cuts
                    // as test partitions
                    //__________________________________________________________
                    meta_list            half;
                    for(partition *p=plist.head;p;p=p->next)
                    {
                        if(2==p->size) half.append(p);
                    }
                    if(half.size<=0) throw exception("unexpected no half partition!");
                    std::cerr << "Using #half_partition=" << half.size << std::endl;
                    

                    return sequential->sizes;
                }
                else
                {
                    //__________________________________________________________
                    //
                    // only one possible way
                    //__________________________________________________________
                    return sequential->sizes;
                }
            }

        };

        typedef partition<coord1D> partition1D;
        typedef partition<coord2D> partition2D;
        typedef partition<coord3D> partition3D;
    }
}

#endif

