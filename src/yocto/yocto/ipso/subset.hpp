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

            const size_t          rank;       //!< global (aka MPI) rank
            const COORD           ranks;      //!< local ranks
            const patch_type      inner;      //!< inner working patch
            const patch_type      outer;      //!< outer patch, with swap zones
            const swaps_list      local[DIM]; //!< local swaps, 0 or pair by dimension
            const swaps_list      async[DIM]; //!< async swaps
            const swaps_addr_list locals;     //!< locals collection
            const swaps_addr_list asyncs;     //!< asyncs collection
            subset               *next;       //!< for subset::list
            subset               *prev;       //!< for subset::list
            const score_t         score;      //!< (inner.items,num_async,num_local)




            inline virtual ~subset() throw() {}

            //! build the outer layout and the swaps
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
            locals(),
            asyncs(),
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


                //______________________________________________________________
                //
                // build outer patch and straight swaps
                //______________________________________________________________
#define         YOCTO_IPSO_LOWER_SWAPS(KIND) \
do { const unsigned flag = swaps::dim2pos(dim,-1); _##KIND.push_back( new swaps(rank, full.prev_rank(ranks,dim), layers, flag) ); } while(false)
#define         YOCTO_IPSO_UPPER_SWAPS(KIND) \
do { const unsigned flag = swaps::dim2pos(dim, 1); _##KIND.push_back( new swaps(rank, full.next_rank(ranks,dim), layers, flag) ); } while(false)

                COORD        lower = inner.lower;
                COORD        upper = inner.upper;
                for(size_t dim=0;dim<DIM;++dim)
                {
                    swaps_list  & _async = (swaps_list  &)(async[dim]);
                    swaps_list  & _local = (swaps_list  &)(local[dim]);

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
                // rebuild outer patch
                new ((void*)&outer) patch_type(lower,upper);

                //______________________________________________________________
                //
                // Pass 2: loading straight swaps
                //______________________________________________________________
                for(int dim=DIM-1;dim>=0;--dim)
                {
                    async[dim].__sort();
                    for(swaps *swp = async[dim].tail; swp; swp=swp->prev )
                    {
                        swp->load(inner,outer,build);
                    }
                }

                for(size_t dim=0;dim<DIM;++dim)
                {
                    for(swaps *swp = local[dim].tail; swp; swp=swp->prev )
                    {
                        swp->load(inner,outer,build);
                    }
                }

                register_all_swaps();


                //______________________________________________________________
                //
                // Pass 3: loading cross swaps
                //______________________________________________________________
                load_cross_swaps(full,layers,pbcs,build);

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
                for( const swaps_addr_node *swp = asyncs.head;swp;swp=swp->next)
                {
                    swp->addr->allocate_for(block_size);
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
                for(const swaps_addr_node *swp = asyncs.head;swp;swp=swp->next )
                {
                    swp->addr->iobuf.reset();
                }
            }

            //__________________________________________________________________
            //
            //! store data to send and copy local
            //__________________________________________________________________
            inline void __local_xch( field_info &f ) throw()
            {
                for(size_t dim=0;dim<DIM;++dim)
                {
                    f.swap_local(local[dim]);
                }
            }

            inline void __async_sto(field_info &f) throw()
            {
                for(const swaps_addr_node *swp = asyncs.head;swp;swp=swp->next )
                {
                    f.swap_store(**swp);
                }
            }


            inline void sync_store( field_info &f ) throw()
            {
                __local_xch(f);
                __async_sto(f);
            }

            inline void sync_store( fields &fvar ) throw()
            {
                for(size_t i=fvar.size();i>0;--i)
                {
                    field_info &f = *fvar[i];
                    __local_xch(f);
                    __async_sto(f);
                }
            }



            //__________________________________________________________________
            //
            //! sync_store procedure end: prepare all iobuf for I/O
            //__________________________________________________________________
            inline void sync_store_end() throw()
            {
                for(const swaps_addr_node *swp = asyncs.head;swp;swp=swp->next )
                {
                    swp->addr->iobuf.prepare_recv();
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
                for(const swaps_addr_node *swp = asyncs.head;swp;swp=swp->next )
                {
                    f.swap_query(**swp);
                }
            }

            inline void sync_query( fields &fvar ) throw()
            {
                for(size_t i=fvar.size();i>0;--i)
                {
                    sync_query(*fvar[i]);
                }
            }

            
            inline void register_all_swaps()
            {
                assert(0==asyncs.size);
                assert(0==locals.size);
                
                swaps_addr_list &_asyncs = (swaps_addr_list &)asyncs;
                swaps_addr_list &_locals = (swaps_addr_list &)locals;
                for(size_t dim=0;dim<DIM;++dim)
                {
                    for(swaps *swp = local[dim].head;swp;swp=swp->next)
                    {
                        _locals.append(swp);
                    }
                    
                    for(swaps *swp = async[dim].head;swp;swp=swp->next )
                    {
                        _asyncs.append(swp);
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
                size_t num_async = 0;
                size_t num_local = 0;
                for(size_t dim=0;dim<DIM;++dim)
                {
                    num_async += async[dim].counts();
                    num_local += local[dim].counts();
                }
                new ((void*)&score) score_t( inner.items,num_async,num_local);
            }

          

            //! 1D constructor, to split
            inline subset(const size_t      r_id,
                          const patch_type &__inner,
                          const patch_type &__outer) :
            rank(r_id),
            ranks(r_id),
            inner( __inner ),
            outer( __outer ),
            local(),
            async(),
            locals(),
            asyncs(),
            next(0),
            prev(0),
            score()
            {
            }

        };

        template <typename COORD>
        inline void split(const subset<COORD>   &source,
                          subset<coord1D>::list &subs )
        {
            const coord1D *inner_lower = (const coord1D *) & source.inner.lower;
            const coord1D *inner_upper = (const coord1D *) & source.inner.upper;
            const coord1D *outer_lower = (const coord1D *) & source.outer.lower;
            const coord1D *outer_upper = (const coord1D *) & source.outer.upper;
            
            for(size_t dim=0;dim<YOCTO_IPSO_DIM_OF(COORD);++dim)
            {
                // take patch by dimension
                const patch1D    inner( inner_lower[dim], inner_upper[dim] );
                const patch1D    outer( outer_lower[dim], outer_upper[dim] );
                subset<coord1D> *sub = new subset<coord1D>(source.rank,inner,outer);
                subs.push_back(sub);
                
                // duplicate topology
                for(const swaps *swp = source.local[dim].head;swp;swp=swp->next)
                {
                    
                }
                
                for(const swaps *swp = source.async[dim].head;swp;swp=swp->next)
                {
                    
                }
                
                sub->register_all_swaps();
            }
        }
        

        
    }
}

#endif

