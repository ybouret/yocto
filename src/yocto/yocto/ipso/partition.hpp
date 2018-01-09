#ifndef YOCTO_IPSO_PARTITION_INCLUDED
#define YOCTO_IPSO_PARTITION_INCLUDED 1

#include "yocto/ipso/domain.hpp"

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


            const COORD sizes; //!< keep track of global sizes
            partition  *next;
            partition  *prev;
            mpq         score;

            explicit partition(const divider<COORD>  &full,
                               const size_t           ng,
                               const COORD            pbcs,
                               const bool             build=false) :
            domains(),
            sizes(full.sizes),
            next(0),
            prev(0),
            score()
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


            static inline
            int compare_by_score(const partition *lhs, const partition *rhs, void *) throw()
            {
                const mpq &nl = lhs->score;
                const mpq &nr = rhs->score;
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
                        return (COORD::lexicompare(lhs->sizes,rhs->sizes));
                    }
                }
            }


            //! compute alpha relative to sequential timing
            inline
            mpq compute_alpha(const mpn &sequentialItems) const
            {
                assert(this->size>0);
                // initialize with the first domain
                const domain_type *d = this->head;
                assert(d->load.async>0);
                assert(d->load.items<=sequentialItems);
                mpq alpha = d->load.compute_alpha(sequentialItems);
                for(d=d->next;d;d=d->next)
                {
                    const mpq tmp = d->load.compute_alpha(sequentialItems);
                    if(tmp<alpha) alpha=tmp;
                }
                return alpha;
            }

            //! compute score form items and alpha
            inline
            void compute_score(const mpq &alpha)
            {
                const domain_type *d = this->head;
                score = d->load.compute_score(alpha);
                for(d=d->next;d;d=d->next)
                {
                    const mpq tmp = d->load.compute_score(alpha);
                    if(tmp>score) score=tmp;
                }
            }


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(partition);
            static inline
            COORD compute_optimal_from( list &plist )
            {
                assert(plist.size>0);
                partition   *seq  = plist.head; assert(1==seq->size);
                const mpn   &Is   = seq->head->load.items;
                std::cerr << "#sequential=" << Is << std::endl;
                if(plist.size>1)
                {
                    // take the slowest parallel partition
                    partition *par   = seq->next; assert(par);
                    const mpq  alpha = par->compute_alpha(Is);
                    std::cerr << "alpha=" << alpha << " (" << alpha.to_double() << ")" << std::endl;

                    // compute the score of each partition
                    for(partition *p=plist.head;p;p=p->next)
                    {
                        p->compute_score(alpha);
                        //std::cerr << p->sizes << " => score=" << p->score.to_double() << std::endl;
                    }

                    // then rank according to score and sizes
                    core::merging<partition>::sort(plist,partition<coord2D>::compare_by_score, NULL);
                    for(partition *p=plist.head;p;p=p->next)
                    {
                        std::cerr << p->sizes << " => score=" << p->score.to_double() << std::endl;
                    }

                    // and the winner is...
                    return plist.head->sizes;
                }
                else
                {
                    return seq->sizes; //!< only one possible partition...
                }
            }

        };

        typedef partition<coord1D> partition1D;
        typedef partition<coord2D> partition2D;
        typedef partition<coord3D> partition3D;
    }
}

#endif

