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

            typedef domain<COORD>                 domain_type;
            typedef typename domain_type::list    domains;
            typedef core::list_of_cpp<partition>  list;
            typedef addr_list<partition>          meta_list;
            typedef addr_node<partition>          meta_node;
            
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
                for(size_t rank=0;rank<full.size;++rank)
                {
                    this->push_back( new domain_type(full,rank,ng,pbcs,build) );
                }
            }

            virtual ~partition() throw()
            {
            }

            //! build the possible partitions
            /**
             uses the unified call after that
             */
            static COORD optimal(const size_t        max_cpus,
                                 const size_t        num_ghosts,
                                 const patch<COORD> &zone,
                                 const COORD         pbcs,
                                 COORD              *fallback,
                                 FILE               *fp);

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
            int compare_extended_coords(const COORD &lsz, const COORD &rsz) throw()
            {
                coord1D L[1+DIM] = { __coord_sum(lsz) };
                coord1D R[1+DIM] = { __coord_sum(rsz) };
                for(size_t dim=0;dim<DIM;++dim)
                {
                    L[1+dim] = __coord(lsz,dim);
                    R[1+dim] = __coord(rsz,dim);
                }
                return __lexicographic_inc_fwd<coord1D,1+DIM>(L,R);
            }

            static inline
            int compare_by_wxch(const partition *lhs, const partition *rhs, void *) throw()
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
                        return compare_extended_coords(lhs->sizes,rhs->sizes);
                    }
                }
            }

            static inline
            int compare_by_copy(const partition *lhs, const partition *rhs, void *) throw()
            {
                const metrics::type &cl = lhs->rates.copy;
                const metrics::type &cr = rhs->rates.copy;
                if(cl<cr)
                {
                    return -1;
                }
                else
                {
                    if(cr<cl)
                    {
                        return 1;
                    }
                    else
                    {
                        return compare_extended_coords(lhs->sizes,rhs->sizes);
                    }
                }
            }


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(partition);
            //__________________________________________________________________
            //
            // find partition(s) with 2 domains
            //__________________________________________________________________
            static inline void find_half_partitions(meta_list &half,
                                                    list      &plist)
            {
                for(partition *p=plist.head;p;p=p->next)
                {
                    if(2==p->size) {  half.append(p); }
                }
                if(half.size<=0) throw exception("no half size partition%u",unsigned(DIM));
            }

            //__________________________________________________________________
            //
            // compute timing parameters for those partitions
            //__________________________________________________________________
            static inline void find_half_params(cycle_params    &params,
                                                const meta_list &half,
                                                const metrics   &seq )
            {
                const meta_node *mp = half.head;
                mp->addr->compute_params(params,seq);
                for(mp = mp->next; mp; mp=mp->next)
                {
                    cycle_params tmp;
                    mp->addr->compute_params(tmp,seq);
                    params.keep_min(tmp);
                }
            }

            //__________________________________________________________________
            //
            // integrated call to compute parameters for half partitions
            //__________________________________________________________________
            static inline void find_params(cycle_params  &params,
                                           list          &plist,
                                           const metrics &seq)
            {
                meta_list half;
                find_half_partitions(half,plist);
                find_half_params(params,half,seq);
            }

            //__________________________________________________________________
            //
            // retains partitions with the same optimial size
            // and try to retain greater sizes for fallaback
            //__________________________________________________________________
            static inline void retain_cpus(const size_t  cpus,
                                           list         &plist,
                                           list         &flist )
            {
                assert(0==flist.size);
                size_t max_cpus = 0;
                list   tmp;
                // first pass: detect #max_cpus and remove those under #cpus
                while(plist.size>0)
                {
                    partition *p = plist.pop_front();
                    max_cpus = max_of(max_cpus,p->size);
                    if(p->size<cpus) { delete p;        }
                    else             { tmp.push_back(p);}
                }

                // second pass: retain #cpus in plist in #max_cpus in flist
                while(tmp.size>0)
                {
                    partition *p = tmp.pop_front();
                    assert(p->size>=cpus);
                    if(p->size>cpus)
                    {
                        if(max_cpus==p->size)
                        {
                            flist.push_back(p);
                        }
                        else
                        {
                            delete p;
                        }
                    }
                    else
                    {
                        plist.push_back(p);
                    }
                }
                
            }

            inline void print_rates(FILE *fp) const
            {
                 fprintf(fp,"for "); __coord_printf(fp,sizes); fprintf(fp, "\trates= "); rates.print(fp); fprintf(fp,"\n");
            }

            static inline
            COORD compute_optimal_from(list         &plist,
                                       COORD        *fallback,
                                       FILE         *fp)
            {
                
                //______________________________________________________________
                //
                // rank by cores/splitting: first is one core, the slowest
                //______________________________________________________________
                core::merging<partition>::template sort<core::list_of>(plist,partition::compare_by_cores, NULL);
                if(fp)
                {
                    fprintf(fp,"#partitions=%u\n", unsigned(plist.size));
                    for(const partition *p = plist.head;p;p=p->next)
                    {
                        fprintf(fp,"accepting sizes="); __coord_printf(fp,p->sizes); fprintf(fp," | #cpu=%u\n", unsigned(p->size));
                        for(const domain_type *d = p->head; d; d=d->next)
                        {
                            fprintf(fp,"\t"); __coord_printf(fp,d->ranks);
                            fprintf(fp," : "); d->load.print(fp);
                            fprintf(fp,"\n");
                        }
                    }
                }

                //______________________________________________________________
                //
                // then use information
                //______________________________________________________________
                partition     *sequential  = plist.head; assert(1==sequential->size);
                const metrics &seq         = sequential->head->load;
                const COORD    seq_sizes   = sequential->sizes;

                if(fp) { fprintf(fp,"#sequential="); seq.print(fp); fprintf(fp,"\n"); }
                if(plist.size>1)
                {
                    //__________________________________________________________
                    //
                    // compute params based on half partition(s)
                    //__________________________________________________________

                    cycle_params params;
                    find_params(params,plist,seq);
                    if(fp) { fprintf(fp,"params: w=%g l=%g\n", params.w.to_double(), params.l.to_double() ); }

                    //__________________________________________________________
                    //
                    // then compute rates to see if further parallelism is ok
                    //__________________________________________________________
                    for(partition *p=plist.head;p;p=p->next)
                    {
                        p->compute_rates(params);
                    }

                    //__________________________________________________________
                    //
                    // sort according to rates and topology
                    //__________________________________________________________
                    core::merging<partition>::template sort<core::list_of>(plist,compare_by_wxch,NULL);
                    if(fp)
                    {
                        for(const partition *p=plist.head;p;p=p->next)
                        {
                            p->print_rates(fp);
                        }
                    }


                    //__________________________________________________________
                    //
                    // keep above optimal cpu counts
                    //__________________________________________________________

                    const size_t cpus = plist.head->size;
                    list          flist; //!< fallback list
                    if(fp) fprintf(fp,"-- retaining #cpus>=%u\n", unsigned(cpus));
                    retain_cpus(cpus,plist,flist);

                    if(fp)
                    {
                        fprintf(fp,"-- fallback partitions:\n");
                        for(const partition *p=flist.head;p;p=p->next)
                        {
                            p->print_rates(fp);
                        }

                        fprintf(fp,"-- working  partitions:\n");
                        for(const partition *p=plist.head;p;p=p->next)
                        {
                            p->print_rates(fp);
                        }
                    }

                    // then order sub lists by copy/extended coord
                    core::merging<partition>::template sort<core::list_of>(plist,compare_by_copy,NULL);
                    core::merging<partition>::template sort<core::list_of>(flist,compare_by_copy,NULL);
                    const partition *pOptimize = plist.head;
                    if(fallback)
                    {
                        *fallback = ( (flist.size>0) ? flist.head : pOptimize)->sizes;
                    }
                    return pOptimize->sizes;
                }
                else
                {
                    //__________________________________________________________
                    //
                    // only one possible way
                    //__________________________________________________________
                    return seq_sizes;
                }
            }

        };

        typedef partition<coord1D> partition1D;
        typedef partition<coord2D> partition2D;
        typedef partition<coord3D> partition3D;
    }
}

#endif

