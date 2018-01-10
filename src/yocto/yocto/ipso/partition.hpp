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








        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(partition);
#if 0
            static inline
            COORD compute_optimal_from( list &plist )
            {
                assert(plist.size>0);
                partition   *seq  = plist.head; assert(1==seq->size);
                const mpn   &Is   = seq->head->load.items;
                std::cerr << "#sequential=" << Is << std::endl;
                if(plist.size>1)
                {
                    // take the slowest parallel partition with 2 cuts
                    partition *par   = seq->next; assert(par); assert(2==par->size);
                    size_t     count = 1;
                    mpq        alpha = par->compute_alpha(Is);
                    std::cerr << "alpha" << count << "=" << alpha << " (" << alpha.to_double() << ")" << std::endl;
                    for(par=par->next;(NULL!=par)&&(2==par->size);par=par->next)
                    {
                        ++count;
                        const mpq tmp = par->compute_alpha(Is);
                        std::cerr << "alpha" << count << "=" << tmp << " (" << tmp.to_double() << ")" << std::endl;
                        if(tmp<alpha) alpha=tmp;
                    }

                    // compute the score of each partition
                    for(partition *p=plist.head;p;p=p->next)
                    {
                        p->compute_score(alpha);
                    }

                    // then rank according to score and sizes
                    core::merging<partition>::sort(plist,partition<coord2D>::compare_by_score, NULL);
                    for(partition *p=plist.head;p;p=p->next)
                    {
                        std::cerr << p->sizes << " => score=" << p->score << "\t=\t" << p->score.to_double() << std::endl;
                    }

                    // and the winner is...
                    return plist.head->sizes;
                }
                else
                {
                    return seq->sizes; //!< only one possible partition...
                }
            }

            static inline
            COORD compute_optimal_v2_from( list &plist )
            {
                assert(plist.size>0);
                partition    *seq  = plist.head; assert(1==seq->size);
                const metrics smx  = seq->head->load;
                std::cerr << "#sequential=" << smx.items << std::endl;
                if(plist.size>1)
                {
                    // take the slowest parallel partitions with 2 cuts
                    partition *par   = seq->next; assert(par); assert(2==par->size);
                    size_t     count = 1;
                    copy_rates rates;
                    par->compute_copy_rates(rates,smx);
                    std::cerr << "rates" << count << "=" << rates << std::endl;
                    for(par=par->next;(NULL!=par)&&(2==par->size);par=par->next)
                    {
                        ++count;
                        copy_rates tmp;
                        par->compute_copy_rates(tmp,smx);
                        std::cerr << "rates" << count << "=" << tmp << std::endl;
                        rates.keep_min_of(tmp);
                    }
                    std::cerr << "using rates=" << rates << std::endl;
                    
                    // compute the score of each partition
                    for(partition *p=plist.head;p;p=p->next)
                    {
                        //p->compute_score(alpha);
                    }

                    // then rank according to score and sizes
                    //core::merging<partition>::sort(plist,partition<coord2D>::compare_by_score, NULL);
                    for(partition *p=plist.head;p;p=p->next)
                    {
                        std::cerr << p->sizes << " => score=" << p->score << "\t=\t" << p->score.to_double() << std::endl;
                    }

                    // and the winner is...
                    return plist.head->sizes;
                }
                else
                {
                    return seq->sizes; //!< only one possible partition...
                }
            }
#endif

        };

        typedef partition<coord1D> partition1D;
        typedef partition<coord2D> partition2D;
        typedef partition<coord3D> partition3D;
    }
}

#endif

