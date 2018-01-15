#ifndef YOCTO_IPSO_PARTITION_INCLUDED
#define YOCTO_IPSO_PARTITION_INCLUDED 1

#include "yocto/ipso/domain.hpp"
#include "yocto/sequence/addr-list.hpp"
#include "yocto/container/tuple.hpp"

namespace yocto
{
    namespace ipso
    {

        YOCTO_PAIR_DECL(YOCTO_TUPLE_STANDARD,cycle_params,metrics::type,w,metrics::type,l);
        typedef metrics::type type;
        inline cycle_params() : w(), l() {}
        inline void compute_from_sequential(const metrics &seq, const metrics &dom)
        {
            const type dW = seq.items - dom.items;
            const type dL = seq.local - dom.local;
            const type dA = 1;
            const type A  = dom.async + dA;
            w = dW/A;
            l = dL/A;
        }

        inline void keep_minimum(const cycle_params &other)
        {
            if(other.w<w) w=other.w;
            if(other.l<l) l=other.l;
        }
        YOCTO_PAIR_END();


        YOCTO_PAIR_DECL(YOCTO_TUPLE_STANDARD,cycle_rates,metrics::type,wxch,metrics::type,copy);
        typedef metrics::type type;
        inline cycle_rates() : wxch(), copy() {}
        inline void compute_from_params(const cycle_params &params, const metrics &dom)
        {
            wxch = dom.items + params.w * dom.async;
            copy = dom.local + params.l * dom.async;
        }

        inline void keep_maximum(const cycle_rates &other)
        {
            if(wxch<other.wxch) wxch = other.wxch;
            if(copy<other.copy) copy = other.copy;
        }
        YOCTO_PAIR_END();

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

            const COORD   sizes; //!< keep track of global sizes
            partition    *next;
            partition    *prev;
            metrics::type score;

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

            void compute_params( cycle_params &params, const metrics &seq ) const
            {
                const domain_type *d = this->head;
                params.compute_from_sequential(seq,d->load);
                for(d=d->next;d;d=d->next)
                {
                    cycle_params tmp;
                    tmp.compute_from_sequential(seq,d->load);
                    params.keep_minimum(tmp);
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

                    cycle_params params;
                    {
                        meta_node *mp = half.head;
                        mp->addr->compute_params(params,seq);
                        std::cerr << "\t\tparams=" << params << std::endl;
                        for(mp=mp->next;mp;mp=mp->next)
                        {
                            cycle_params tmp;
                            mp->addr->compute_params(tmp,seq);
                            std::cerr << "\t\tparams=" << tmp << std::endl;
                            params.keep_minimum(tmp);
                        }
                    }
                    std::cerr << "\tparams=" << params << std::endl;


                    std::cerr << "computing rates: " << std::endl;
                    for(partition *p=plist.head;p;p=p->next)
                    {
                        std::cerr << "for " << p->sizes << std::endl;
                        cycle_rates prate;
                        for(const domain_type *d = p->head;d;d=d->next)
                        {
                            cycle_rates rates;
                            rates.compute_from_params(params,d->load);
                            std::cerr << "\t\t" << d->load << " => wxch=" << rates.wxch.to_double() << ", copy=" << rates.copy.to_double() << std::endl;
                            prate.keep_maximum(rates);
                        }
                        std::cerr << "\tprate: " << "wxch=" << prate.wxch.to_double() << ", copy=" << prate.copy.to_double() << std::endl;
                        p->score = prate.wxch + prate.copy;
                        std::cerr << "\tscore=" << p->score.to_double() << std::endl;
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

