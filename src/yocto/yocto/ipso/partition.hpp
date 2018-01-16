#ifndef YOCTO_IPSO_PARTITION_INCLUDED
#define YOCTO_IPSO_PARTITION_INCLUDED 1

#include "yocto/ipso/domain.hpp"
#include "yocto/sequence/addr-list.hpp"
#include "yocto/container/tuple.hpp"

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

            const COORD        sizes; //!< keep track of global sizes
            partition         *next;
            partition         *prev;
            metrics::type      score;
            const domain_type *largest;


            explicit partition(const divider<COORD>  &full,
                               const size_t           ng,
                               const COORD            pbcs,
                               const bool             build=false) :
            domains(),
            sizes(full.sizes),
            next(0),
            prev(0),
            score(),
            largest(0)
            {
                for(size_t rank=0;rank<full.size;++rank)
                {
                    this->push_back( new domain_type(full,rank,ng,pbcs,build) );
                }
                const domain_type *dom = this->head;
                largest = dom;
                for(dom=dom->next;dom;dom=dom->next)
                {
                    if(metrics::compare(dom->load,largest->load)>0)
                    {
                        largest = dom;
                    }
                }
                //std::cerr << "Largest of " << sizes << "=" << largest->load << std::endl;
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

                const metrics &seq         = sequential->head->load;
                std::cerr << "#sequential=" << seq << std::endl;
                if(plist.size>1)
                {
                    for(partition *p=sequential->next;p;p=p->next)
                    {
                        assert(p->size>1);
                        std::cerr << "for " << p->sizes << "\t:";
                        const metrics::type min_W = sequential->largest->load.items/(p->size-1);
                        const metrics::type min_L = sequential->largest->load.local/(p->size-1);
                        // std::cerr << "\tmin_W=" << min_W.to_double() << ", min_L=" << min_L.to_double() << std::endl;
                        const metrics::type dW = (sequential->largest->load.items - p->largest->load.items);
                        const metrics::type dL = (sequential->largest->load.local - p->largest->load.local);
                        const metrics::type dA = 1;
                        const metrics::type A  = p->largest->load.async+dA;
                        const metrics::type w  = dW/A;
                        const metrics::type l  = dL/A;
                        //std::cerr << "\tdW=" << dW.to_double() << ", dL=" << dL.to_double() << std::endl;
                        std::cerr << "\talpha <= " << w.to_double() <<  "+lambda*(" << l.to_double() << ")\t" << w.to_double()+l.to_double() << std::endl;
                    }
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

