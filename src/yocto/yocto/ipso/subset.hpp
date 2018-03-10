#ifndef YOCTO_IPSO_SUBSET_INCLUDED
#define YOCTO_IPSO_SUBSET_INCLUDED 1


#include "yocto/ipso/swaps.hpp"
#include "yocto/ipso/divide.hpp"
#include "yocto/ipso/fields.hpp"

#include "yocto/sort/merge.hpp"

namespace yocto
{
    namespace ipso
    {
        typedef point3d<size_t> score_t;
        
        template <typename COORD>
        class subset : public object
        {
        public:

            typedef core::list_of_cpp<subset> list;
            typedef patch<COORD>              patch_type;
            static const size_t               DIM = YOCTO_IPSO_DIM_OF(COORD);

            const size_t      rank;
            const COORD       ranks;
            const patch_type  inner;
            const patch_type  outer;
            const swaps_list  local; //!< local swaps
            const swaps_list  async; //!< async swaps
            //const swaps_table links; //!< async, ranked by target
            subset           *next;  //!< for subset::list
            subset           *prev;  //!< for subset::list
            const score_t     score; //!< (inner.items,num_async,num_local)




            inline virtual ~subset() throw() {}

            //! build the outer layout and the straight swaps
            /**
             \param full   the full patch divider
             \param r_id   the subset global rank
             \param layers the number of extra layers
             \param pbcs   the periodic boundary conditions
             \param build  if the send/recv offsets are to be built
             */
            inline explicit subset(const divider<COORD> &full,
                                   const size_t          r_id,
                                   const size_t          layers,
                                   const COORD           pbcs,
                                   const bool            build) :
            rank(r_id),
            ranks(),
            inner( full(rank, (COORD *)&ranks) ),
            outer( inner ),
            local(),
            async(),
            next(0),
            prev(0),
            score()
            {
                //______________________________________________________________
                //
                // checking swaps availability
                //______________________________________________________________
                const bool has_swaps = (layers>0);
                if(!has_swaps)
                {
                    set_score();
                    return;
                }

                //______________________________________________________________
                //
                // checking not too many layers
                //______________________________________________________________
                for(size_t dim=0;dim<DIM;++dim)
                {
                    const size_t ww = __coord(inner.width,dim);
                    if(layers>ww)
                    {
                        throw libc::exception(EDOM,"subset%uD: layers=%d>width.%s=%d", unsigned(DIM), int(layers), __coord_name(dim),int(ww));
                    }
                }

                swaps::list & _local = (swaps::list &)local;
                swaps_list  & _async = (swaps_list  &)async;

                //______________________________________________________________
                //
                // build outer patch and straight swaps
                //______________________________________________________________
#define         YOCTO_IPSO_LOWER_SWAPS(KIND) \
do { const unsigned flag = swaps::dim2pos(dim,-1); /*_##KIND##_flags |= flag;*/ _##KIND.push_back( new swaps(rank, full.prev_rank(ranks,dim), layers, flag) ); } while(false)
#define         YOCTO_IPSO_UPPER_SWAPS(KIND) \
do { const unsigned flag = swaps::dim2pos(dim, 1); /*_##KIND##_flags |= flag;*/ _##KIND.push_back( new swaps(rank, full.next_rank(ranks,dim), layers, flag) ); } while(false)


                COORD lower = inner.lower;
                COORD upper = inner.upper;
                //unsigned & _local_flags = (unsigned &)local_flags;
                //unsigned & _async_flags = (unsigned &)async_flags;
                for(size_t dim=0;dim<DIM;++dim)
                {
                    //__________________________________________________________
                    //
                    // Pass 1: computing outer patch
                    //__________________________________________________________
                    const bool    periodic = (0!=__coord(pbcs,dim));
                    coord1D      &lo       = __coord(lower,dim);
                    coord1D      &up       = __coord(upper,dim);
                    const coord1D sz       = __coord(full.sizes,dim); assert(sz>0);
                    const bool    parallel = (sz>1);
                    const coord1D rk       = __coord(ranks,dim);      assert(rk<sz);
                    const coord1D last     = __coord(full.lasts,dim); assert(sz-1==last);
                    if(periodic)
                    {
                        ////////////////////////////////////////////////////////
                        //
                        // PERIODIC CODE
                        //
                        ////////////////////////////////////////////////////////
                        lo-=layers;
                        up+=layers;
                        if(parallel)
                        {
                            //__________________________________________________
                            //
                            // PARALLEL in that dimension: always async
                            //__________________________________________________
                            if(has_swaps)
                            {
                                YOCTO_IPSO_LOWER_SWAPS(async);
                                YOCTO_IPSO_UPPER_SWAPS(async);
                                //async.push_back( YOCTO_IPSO_LOWER_SWAPS );
                                //async.push_back( YOCTO_IPSO_UPPER_SWAPS );
                            }
                        }
                        else
                        {
                            //__________________________________________________
                            //
                            // SEQUENTIAL in that dimension: always local PAIR
                            //__________________________________________________
                            assert(1==sz);

                            YOCTO_IPSO_LOWER_SWAPS(local);
                            YOCTO_IPSO_UPPER_SWAPS(local);
                            //local.push_back( YOCTO_IPSO_LOWER_SWAPS );
                            //local.push_back( YOCTO_IPSO_UPPER_SWAPS );
                        }
                    }
                    else
                    {
                        ////////////////////////////////////////////////////////
                        //
                        // NOT PERIODIC CODE
                        //
                        ////////////////////////////////////////////////////////
                        if(parallel)
                        {
                            if(0==rk)
                            {
                                //______________________________________________
                                //
                                // not periodic @first
                                //______________________________________________
                                up += layers;
                                YOCTO_IPSO_UPPER_SWAPS(async);
                            }
                            else
                            {
                                if(last==rk)
                                {
                                    //__________________________________________
                                    //
                                    // not periodic @last
                                    //__________________________________________
                                    lo -= layers;
                                    YOCTO_IPSO_LOWER_SWAPS(async);
                                }
                                else
                                {
                                    // in bulk
                                    lo -= layers;
                                    up += layers;
                                    YOCTO_IPSO_LOWER_SWAPS(async);
                                    YOCTO_IPSO_UPPER_SWAPS(async);
                                }
                            }
                        }
                        else
                        {
                            // nothing to do
                        }
                    }

                }
                new ((void*)&outer) patch_type(lower,upper);

                //______________________________________________________________
                //
                // Pass 2: loading straight swaps
                //______________________________________________________________
                for(swaps *swp = async.tail; swp; swp=swp->prev )
                {
                    swp->load(inner,outer,build);
                }

                for(swaps *swp = local.tail; swp; swp=swp->prev )
                {
                    swp->load(inner,outer,build);
                }

                //______________________________________________________________
                //
                // Pass 3: loading cross swaps
                //______________________________________________________________
                load_cross_swaps(full,layers,pbcs,build);

                if(build)
                {
                    _async.join();
                }

                
                //______________________________________________________________
                //
                // Pass 4: setting scores
                //______________________________________________________________
                set_score();

                
            }



            //__________________________________________________________________
            //
            //! allocating memory for swaps, ASYNC only...
            //__________________________________________________________________
            inline void allocate_swaps_for( const size_t block_size )
            {
                swaps::list & _async = (swaps::list &)async;
                for( swaps *swp = _async.head;swp;swp=swp->next)
                {
                    swp->allocate_for(block_size);
                }
            }

            //! allocating memory for one field
            inline void allocate_swaps_for( const field_info &F )
            {
                allocate_swaps_for(F.item_size);
            }

            //! allocating memory for different fields
            inline void allocate_swaps_for( const fields &fvar )
            {
                allocate_swaps_for(fvar.block_size());
            }

            //__________________________________________________________________
            //
            //! copy local data for one field
            //__________________________________________________________________
            inline void copy_local( field_info &F ) throw()
            {
                F.swap_local(local);
            }

            //! copy local data for different fields
            inline void copy_local( fields &fvar ) throw()
            {
                for(size_t i=fvar.size();i>0;--i)
                {
                    fvar[i]->swap_local(local);
                }
            }

            //__________________________________________________________________
            //
            //
            //! sync_store procedure begin: reset all async buffers
            //
            //__________________________________________________________________
            inline void sync_store_begin() throw()
            {
                for(const swaps *swp = async.head;swp;swp=swp->next )
                {
                    swp->iobuf.reset();
                }
            }

            //__________________________________________________________________
            //
            //! store data to send and copy local
            //__________________________________________________________________
            inline void sync_store( field_info &f ) throw()
            {
                f.swap_local(local);
                for(const swaps *swp = async.head;swp;swp=swp->next )
                {
                    f.swap_store(*swp);
                }
            }

            inline void sync_store( fields &fvar ) throw()
            {
                const size_t nvar = fvar.size();
                for(size_t i=nvar;i>0;--i)
                {
                    fvar[i]->swap_local(local);
                }
                for(const swaps *swp = async.head;swp;swp=swp->next )
                {
                    const swaps &s = *swp;
                    for(size_t i=nvar;i>0;--i)
                    {
                        fvar[i]->swap_store(s);
                    }
                }
            }



            //__________________________________________________________________
            //
            //! sync_store procedure end: prepare all iobuf for I/O
            //__________________________________________________________________
            inline void sync_store_end() throw()
            {
                for(const swaps *swp = async.head;swp;swp=swp->next )
                {
                    swp->iobuf.prepare_recv();
                }
            }

            //__________________________________________________________________
            //
            //
            //! wrapper for simpler operations
            //
            //__________________________________________________________________
            inline void sync_start( field_info &F ) throw()
            {
                sync_store_begin();
                sync_store(F);
                sync_store_end();
            }

            //! wrapper for different fields
            inline void sync_start( fields &fvar) throw()
            {
                sync_store_begin();
                sync_store(fvar);
                sync_store_end();
            }

            //__________________________________________________________________
            //
            //
            //! sync_query procedure, once I/O is done
            //
            //__________________________________________________________________
            inline void sync_query( field_info &f ) throw()
            {
                for(const swaps *swp = async.head;swp;swp=swp->next )
                {
                    f.swap_query(*swp);
                }
            }

            inline void sync_query( fields &fvar ) throw()
            {
                const size_t nvar = fvar.size();
                for(const swaps *swp = async.head;swp;swp=swp->next )
                {
                    const swaps &s = *swp;
                    for(size_t i=nvar;i>0;--i)
                    {
                        fvar[i]->swap_query(s);
                    }
                }
            }




        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(subset);
            void load_cross_swaps(const divider<COORD> &full,
                                  const size_t          layers,
                                  const COORD           pbcs,
                                  const bool            build);

            inline void set_score() throw()
            {
                new ((void*)&score) score_t( inner.items,async.counts(),local.counts());
            }


        };


        ////////////////////////////////////////////////////////////////////////
        //
        // automatic list of subsets
        //
        ////////////////////////////////////////////////////////////////////////
        template <typename COORD>
        class subsets : public object, public subset<COORD>::list
        {
        public:
            typedef core::list_of_cpp<subsets> list;


            const COORD   sizes;
            const score_t score;
            subsets      *next;
            subsets      *prev;

            inline virtual ~subsets() throw() {}

            inline explicit subsets(const divider<COORD> &full,
                                    const size_t          layers,
                                    const COORD           pbcs,
                                    const bool            build = false ) :
            subset<COORD>::list(),
            sizes( full.sizes ),
            score(),
            next(0),
            prev(0)
            {
                score_t &max_score = (score_t &)score;
                for(size_t rank=0;rank<full.size;++rank)
                {
                    subset<COORD> *sub = new subset<COORD>(full,rank,layers,pbcs,build);
                    this->push_back(sub);
                    const score_t &sub_score = sub->score;
                    if( __lexicographic_dec_fwd<size_t,3>(&sub_score,&max_score) < 0 )
                    {
                        max_score = sub_score;
                    }
                }
                assert(this->size==size_t(__coord_prod(sizes)));
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(subsets);
        };

        ////////////////////////////////////////////////////////////////////////
        //
        // used to find optimial
        //
        ////////////////////////////////////////////////////////////////////////
        template <typename COORD>
        class mapping : public subsets<COORD>::list
        {
        public:
            static const size_t DIM = YOCTO_IPSO_DIM_OF(COORD);
            const subsets<COORD> * optimal;
            const subsets<COORD> * fallback;

            inline virtual ~mapping() throw() {}
            inline explicit mapping(const size_t        cpus,
                                    const patch<COORD> &full,
                                    const size_t        layers,
                                    const COORD         pbcs) :
            subsets<COORD>::list(),
            optimal(0),
            fallback(0)
            {
                setup(cpus,full,layers,pbcs);
                typename subsets<COORD>::list &self = *this;
                assert(self.size>0);
                core::merging< subsets<COORD> >::sort(self, compare_by_scores, NULL );
                optimal  = self.head; assert(optimal);
                fallback = self.tail; assert(fallback);
                for(const subsets<COORD> *subs=fallback;subs;subs=subs->prev)
                {
                    if(subs->size>=fallback->size)
                    {
                        fallback = subs;
                    }
                }
            }

            static inline
            COORD optimal_sizes_for(const size_t        cpus,
                                    const patch<COORD> &full,
                                    const size_t        layers,
                                    const COORD        &pbcs,
                                    COORD              *fallback)
            {
                const mapping maps(cpus,full,layers,pbcs);
                if(fallback)
                {
                    *fallback = maps.fallback->sizes;
                }
                return maps.optimal->sizes;
            }


        private:
            void setup(const size_t        cpus,
                       const patch<COORD> &full,
                       const size_t        layers,
                       const COORD         pbcs);
            
            static inline int compare_by_scores( const subsets<COORD> *lhs, const subsets<COORD> *rhs, void * ) throw()
            {
                const int ans = __lexicographic_inc_fwd<size_t,3>( &(lhs->score), &(rhs->score) );
                {
                    if(ans!=0)
                    {
                        // score is winning
                        return ans;
                    }
                    else
                    {
                        // same score : compare by increasing sum+coords
                        coord1D L[DIM+1] = { __coord_sum(lhs->sizes) };
                        coord1D R[DIM+1] = { __coord_sum(rhs->sizes) };
                        for(size_t i=0;i<DIM;++i)
                        {
                            L[i+1] = __coord(lhs->sizes,i);
                            R[i+1] = __coord(rhs->sizes,i);
                        }
                        return __lexicographic_inc_fwd<coord1D,DIM+1>(L,R);
                    }
                }
            }
            YOCTO_DISABLE_COPY_AND_ASSIGN(mapping);
        };

    }
}

#endif

