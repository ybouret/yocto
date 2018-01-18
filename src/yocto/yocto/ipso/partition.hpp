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
            cycle_rates        rates;


            explicit partition(const divider<COORD>  &full,
                               const size_t           ng,
                               const COORD            pbcs,
                               const bool             build=false) :
            domains(),
            sizes(full.sizes),
            next(0),
            prev(0),
            rates()
            {
                //std::cerr << "partition " << sizes << std::endl;
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

            inline void compute_params(cycle_params &params, const metrics &seq) const
            {
                const domain_type *dom = this->head;
                params.compute_from(seq,dom->load);
                for(dom=dom->next;dom;dom=dom->next)
                {
                    cycle_params tmp;
                    tmp.compute_from(seq,dom->load);
                    params.keep_min(tmp);
                }
            }

            inline void compute_rates(const cycle_params &params)
            {
                rates.clear();
                for(const domain_type *dom=this->head;dom;dom=dom->next)
                {
                    cycle_rates tmp;
                    tmp.compute_from(params,dom->load);
                    rates.keep_max(tmp);
                }
            }

            static inline
            int compare_by_rates(const partition *lhs, const partition *rhs, void *) throw()
            {
                const metrics::type &wl = lhs->rates.wxch;
                const metrics::type &wr = rhs->rates.wxch;
                if(wl<wr)
                {
                    return -1;
                }
                else
                {
                    if(wr<wl)
                    {
                        return 1;
                    }
                    else
                    {
                        coord1D L[1+DIM] = { __coord_sum(lhs->sizes) };
                        coord1D R[1+DIM] = { __coord_sum(rhs->sizes) };
                        for(size_t dim=0;dim<DIM;++dim)
                        {
                            L[1+dim] = __coord(lhs->sizes,dim);
                            R[1+dim] = __coord(rhs->sizes,dim);
                        }
                        return __lexicographic_inc_fwd<coord1D,1+DIM>(L,R);
                    }
                }
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

            static inline void find_half_params( cycle_params &params, const meta_list &half, const metrics &seq )
            {
                const meta_node *mp = half.head;
                mp->addr->compute_params(params,seq);
                std::cerr << "for " << mp->addr->sizes << " : " << params.w.to_double() << "," << params.l.to_double() << std::endl;
                for(mp = mp->next; mp; mp=mp->next)
                {
                    cycle_params tmp;
                    mp->addr->compute_params(tmp,seq);
                    std::cerr << "for " << mp->addr->sizes << " : " << tmp.w.to_double() << "," << tmp.l.to_double() << std::endl;
                    params.keep_min(tmp);
                }
                std::cerr << "params : " << params.w.to_double() << "," << params.l.to_double() << std::endl;
            }



            static inline
            COORD compute_optimal_from( list &plist )
            {
                partition     *sequential  = plist.head; assert(1==sequential->size);
                const metrics &seq         = sequential->head->load;
                std::cerr << "#sequential=" << seq << std::endl;
                if(plist.size>1)
                {
                    meta_list            half;
                    find_half_partitions(half,plist);

                    cycle_params params;
                    find_half_params(params,half,seq);
                    for(partition *p=plist.head;p;p=p->next)
                    {
                        //std::cerr << "for " << p->sizes;
                        p->compute_rates(params);
                        //std::cerr << "\trates=" << p->rates.wxch.to_double() << "\t+lambda*" << p->rates.copy.to_double() << std::endl;
                    }

                    core::merging<partition>::sort(plist,compare_by_rates,NULL);
                    for(partition *p=plist.head;p;p=p->next)
                    {
                        std::cerr << "for " << p->sizes;
                        p->compute_rates(params);
                        std::cerr << "\trates=" << p->rates.wxch.to_double() << "\t+lambda*" << p->rates.copy.to_double() << "\t| " << p->rates << std::endl;
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

