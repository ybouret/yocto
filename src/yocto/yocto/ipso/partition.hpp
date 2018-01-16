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
            static const size_t DIM = sizeof(COORD)/sizeof(coord1D);

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

            metrics::type compute_alpha(const metrics &seq ) const
            {
                const domain_type  *dom   = this->head;
                metrics::type       alpha = dom->load.compute_alpha(seq);
                for(dom=dom->next;dom;dom=dom->next)
                {
                    metrics::type tmp = dom->load.compute_alpha(seq);
                    if(tmp<alpha) alpha=tmp;
                }
                return alpha;
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(partition);
            static inline void find_half_partitions( meta_list &half, list &plist)
            {
                for(partition *p=plist.head;p;p=p->next)
                {
                    if(2==p->size)
                    {
                        half.append(p);
                    }
                }
                if(half.size<=0) throw exception("no half size partition%u",unsigned(DIM));
            }

            static inline metrics::type find_alpha_from( const meta_list &half, const metrics &seq)
            {
                assert(half.size>0);
                const meta_node *mp    = half.head;
                metrics::type    alpha = mp->addr->compute_alpha(seq);
                for(mp=mp->next;mp;mp=mp->next)
                {
                    const metrics::type tmp = mp->addr->compute_alpha(seq);
                    if(tmp<alpha) alpha=tmp;
                }
                return alpha;
            }

            inline void compute_score(const metrics::type &alpha)
            {
                score = 0;
                for(const domain_type *dom=this->head;dom;dom=dom->next)
                {
                    const metrics::type tmp = dom->load.items + alpha * dom->load.coeff;
                    if(tmp>score) score=tmp;
                }
                std::cerr << "\tscore for " << sizes << "=" << score.to_double() << std::endl;
            }

            static inline
            COORD compute_optimal_from( list &plist )
            {
                partition     *sequential  = plist.head; assert(1==sequential->size);
                const metrics &seq         = sequential->head->load;
                std::cerr << "#sequential=" << seq << std::endl;
                if(plist.size>1)
                {
                    meta_list half;
                    find_half_partitions(half,plist);

                    for(const meta_node *mp=half.head;mp;mp=mp->next)
                    {
                        const metrics::type tmp = mp->addr->compute_alpha(seq);
                        std::cerr << "\t\talpha=" << tmp << "=" << tmp.to_double() << std::endl;
                    }
                    const metrics::type alpha = find_alpha_from(half,seq);
                    std::cerr << "\talpha=" << alpha << "=" << alpha.to_double() << std::endl;
                    for(partition *p=plist.head;p;p=p->next)
                    {
                        p->compute_score(alpha);
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

