#ifndef YOCTO_IPSO_PARTITION_INCLUDED
#define YOCTO_IPSO_PARTITION_INCLUDED 1

#include "yocto/ipso/domain.hpp"
#include "yocto/sequence/addr-list.hpp"

namespace yocto
{
    namespace ipso
    {

        class cycle_params
        {
        public:
            metrics::type a,l;

            cycle_params() : a(), l() {}
            ~cycle_params() throw() {}

            void compute_from(const metrics &seq, const metrics &dom)
            {
                const metrics::type dW   = seq.items - dom.items;
                const metrics::type dL   = seq.local - dom.local;
                const metrics::type dA   = 1;
                metrics::type       A    = dom.async+dA;
                a = dW/A;
                l = dL/A;
            }

            void keep_min_from(const cycle_params &params )
            {
                if(params.a<a) a=params.a;
                if(params.l<l) l=params.l;
            }
            inline friend std::ostream & operator<<( std::ostream &os, const cycle_params &p )
            {
                os << "(a=" << p.a.to_double() << ",l=" << p.l.to_double() << ")";
                return os;
            }
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(cycle_params);
        };

        class cycle_scores
        {
        public:
            metrics::type wxch,copy;
            cycle_scores() : wxch(), copy() {}
            ~cycle_scores() throw() {}

            void compute_from( const metrics &dom, const cycle_params &params )
            {
                wxch = dom.items + params.a * dom.async;
                copy = dom.local + params.l * dom.async;
            }

            void keep_max_from(const cycle_scores &scores )
            {
                //if(wxch<scores.wxch) wxch = scores.wxch;

            }

            inline friend std::ostream & operator<<( std::ostream &os, const cycle_scores &scores )
            {
                os << "(wxch=" << scores.wxch.to_double() << ",copy=" << scores.copy.to_double() << ")";
                return os;
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(cycle_scores);
        };


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

            inline void compute_params(cycle_params   &params,
                                       const metrics &seq_load) const
            {
                const domain_type *dom = this->head;
                params.compute_from(seq_load,dom->load);
                for(dom=dom->next;dom;dom=dom->next)
                {
                    cycle_params tmp;
                    tmp.compute_from(seq_load,dom->load);
                    params.keep_min_from(tmp);
                }
            }

            inline void compute_scores_using(const cycle_params &params)
            {
                std::cerr << "scores for " << sizes << std::endl;
                cycle_scores scores;
                const domain_type *dom = this->head;
                scores.compute_from(dom->load,params);
                std::cerr << "\t" << scores << " <= " << dom->load << std::endl;
                for(dom=dom->next;dom;dom=dom->next)
                {
                    cycle_scores tmp;
                    tmp.compute_from(dom->load,params);
                    std::cerr << "\t" << tmp << " <= " << dom->load << std::endl;

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
                    cycle_params         params;
                    meta_list            half;
                    for(partition *p=plist.head;p;p=p->next)
                    {
                        if(2==p->size) half.append(p);
                    }
                    if(half.size<=0) throw exception("unexpected no half partition!");
                    std::cerr << "Using #half_partition=" << half.size << std::endl;
                    {
                        meta_node   *mp = half.head;
                        size_t       count=1;
                        mp->addr->compute_params(params,seq_load);
                        std::cerr << "\tparams" << count << " = " << params << std::endl;
                        for(mp=mp->next;mp;mp=mp->next)
                        {
                            cycle_params tmp;
                            ++count;
                            mp->addr->compute_params(tmp,seq_load);
                            params.keep_min_from(tmp);
                            std::cerr << "\tparams" << count << " = " << tmp << std::endl;
                        }
                    }
                    std::cerr << "params=" << params << std::endl;

                    //__________________________________________________________
                    //
                    // compute all scores
                    //__________________________________________________________
                    for(partition *p = plist.head; p; p=p->next)
                    {
                        p->compute_scores_using(params);
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

