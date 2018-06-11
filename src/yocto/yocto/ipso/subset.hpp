#ifndef YOCTO_IPSO_SUBSET_INCLUDED
#define YOCTO_IPSO_SUBSET_INCLUDED 1


#include "yocto/ipso/swaps.hpp"
#include "yocto/ipso/divide.hpp"
#include "yocto/ipso/fields.hpp"
#include "yocto/ipso/vertex.hpp"
#include "yocto/ipso/real-indices.hpp"
#include "yocto/sort/merge.hpp"

namespace yocto
{
    namespace ipso
    {
        typedef point3d<size_t> score_t;

        template <size_t DIM>
        struct subset_metrics;

        template <>
        struct subset_metrics<1>
        {
            static const size_t num_apex = 1;
        };

        template <>
        struct subset_metrics<2>
        {
            static const size_t num_apex = 2;
        };

        template <>
        struct subset_metrics<3>
        {
            static const size_t num_apex = 1;
        };



        template <typename COORD>
        class subset : public object
        {
        public:
            
            typedef core::list_of_cpp<subset> list;
            typedef patch<COORD>              patch_type;
            static const size_t               DIM = YOCTO_IPSO_DIM_OF(COORD);
            typedef subset_metrics<DIM>       metrics;
            static const size_t               APEX = metrics::num_apex;

            const size_t          rank;             //!< global (aka MPI) rank
            const COORD           ranks;            //!< local ranks
            const patch_type      inner;            //!< inner working patch
            const patch_type      outer;            //!< outer patch, with swap zones
            const swaps_list      local[DIM];       //!< local swaps, 0 or pair by dimension
            const swaps_list      async[DIM];       //!< async swaps
            const swaps_list      apex_local[APEX]; //!<
            const swaps_list      apex_async[APEX]; //!<
            
            const swaps_addr_list locals;      //!< locals collection
            const swaps_addr_list asyncs;      //!< asyncs collection
            subset               *next;        //!< for subset::list
            subset               *prev;        //!< for subset::list
            real_indices          realIndices; //!< real indices
            const score_t         score;       //!< (inner.items,num_async,num_local)
            const unsigned        flags;       //!< full flags


            inline virtual ~subset() throw() {
                std::cerr << "~subset: #locals=" << locals.size << ", #asyncs=" << asyncs.size << std::endl;
                for(size_t dim=0;dim<DIM;++dim)
                {
                    std::cerr << "local[" << dim << "].size=" << local[dim].size << std::endl; core::check_list(local[dim]);
                    std::cerr << "async[" << dim << "].size=" << async[dim].size << std::endl; core::check_list(async[dim]);
                }
                for(size_t dim=0;dim<APEX;++dim)
                {
                    std::cerr << "apex_local[" << dim << "].size=" << apex_local[dim].size << std::endl; core::check_list(apex_local[dim]);
                    std::cerr << "apex_async[" << dim << "].size=" << apex_async[dim].size << std::endl; core::check_list(apex_async[dim]);

                }
                core::check_list(locals);
                core::check_list(asyncs);
                }

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
            apex_local(),
            apex_async(),
            locals(),
            asyncs(),
            next(0),
            prev(0),
            realIndices(),
            score(),
            flags(0)
            {

                memset( (void*)&local[0],      0, sizeof(local) );
                memset( (void*)&async[0],      0, sizeof(async) );
                memset( (void*)&apex_local[0], 0, sizeof(apex_local) );
                memset( (void*)&apex_async[0], 0, sizeof(apex_async) );
                //______________________________________________________________
                //
                // checking swaps availability
                //______________________________________________________________
                const bool has_swaps = (layers>0);
                if(!has_swaps)
                {
                    initial_real_indices();
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
                    const bool    periodic     = (0!=__coord(pbcs,dim));
                    coord1D      &lo           = __coord(lower,dim);
                    coord1D      &up           = __coord(upper,dim);
                    const coord1D dim_size     = __coord(full.sizes,dim); assert(dim_size>0);
                    const bool    dim_para     = (dim_size>1);
                    const coord1D dim_rank     = __coord(ranks,dim);      assert(dim_rank<dim_size);
                    const coord1D dim_last     = __coord(full.lasts,dim); assert(dim_size-1==dim_last);
                    const bool    is_dim_first = (0       == dim_rank);
                    const bool    is_dim_last  = (dim_last== dim_rank);
                    if(periodic)
                    {
                        ////////////////////////////////////////////////////////
                        //
                        // PERIODIC CODE for PATCH
                        //
                        ////////////////////////////////////////////////////////
                        lo-=layers;
                        up+=layers;
                        if(dim_para)
                        {
                            //__________________________________________________
                            //
                            // PARALLEL in that dimension: always async PAIR
                            //__________________________________________________
                            YOCTO_IPSO_LOWER_SWAPS(async);
                            YOCTO_IPSO_UPPER_SWAPS(async);
                        }
                        else
                        {
                            //__________________________________________________
                            //
                            // SEQUENTIAL in that dimension: always local PAIR
                            //__________________________________________________
                            assert(1==dim_size);

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
                        if(dim_para)
                        {
                            if(is_dim_first)
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
                                if(is_dim_last)
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

                // and deduce real indices for VisIt...
                compute_real_indices(full,layers,pbcs);

                // compute overall flags from outer/inner
                compute_flags();

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

                //______________________________________________________________
                //
                // Pass 3: loading cross swaps
                //______________________________________________________________
                load_cross_swaps(full,layers,pbcs,build);


                //______________________________________________________________
                //
                // Pass 4: register all swaps for coms
                //______________________________________________________________
                register_all_swaps();

                //______________________________________________________________
                //
                // Pass 5: setting scores (TODO: use cross?)
                //______________________________________________________________
                set_score();
            }



            //__________________________________________________________________
            //
            //
            //! allocating memory for swaps, ASYNC only...
            //
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
            //! store data to send and copy local using swaps_list
            //__________________________________________________________________
            inline void __local_xch( field_info &f ) throw()
            {
                for(size_t dim=0;dim<DIM;++dim)
                {
                    f.swap_local(local[dim]);
                }
                for(size_t diag=0;diag<2;++diag)
                {
                    f.swap_local(apex_local[diag]);
                }
            }

            //__________________________________________________________________
            //
            //! store data into async send zone
            //__________________________________________________________________
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

                // straight
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

                // cross
                for(size_t diag=0;diag<APEX;++diag)
                {
                    for(swaps *swp=apex_local[diag].head;swp;swp=swp->next)
                    {
                        _locals.append(swp);
                    }

                    for(swaps *swp=apex_async[diag].head;swp;swp=swp->next)
                    {
                        _asyncs.append(swp);
                    }
                }
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
            apex_local(),
            apex_async(),
            locals(),
            asyncs(),
            next(0),
            prev(0),
            realIndices(),
            score(),
            flags(0)
            {
            }




        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(subset);

            //! store the apex swap in its slot, and optimize I/O
            inline void push_back_apex( swaps *swp )
            {
                assert(swp);
                assert(this->rank==swp->source);
                const unsigned id = swaps::diagonal_for(swp->pos);
                assert(0==id||1==id);
                swaps_list *l = 0;
                if(swp->source==swp->target)
                {
                    l = & (swaps_list &)(apex_local[id]);
                }
                else
                {
                    l = & (swaps_list &)(apex_async[id]);
                }
                assert(l);
                l->push_back(swp);
                swp->io_check();
            }

            //! dimension dependant function
            void load_cross_swaps(const divider<COORD> &full,
                                  const size_t          layers,
                                  const COORD           pbcs,
                                  const bool            build);

            //__________________________________________________________________
            //
            // compute timing score
            //__________________________________________________________________
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

            //__________________________________________________________________
            //
            // real indices from outer patch
            //__________________________________________________________________
            void initial_real_indices() throw()
            {
                int *minIndices = &realIndices.imin.x;
                int *maxIndices = &realIndices.imax.x;
                for(size_t dim=0;dim<DIM;++dim)
                {
                    minIndices[dim] = 0;
                    maxIndices[dim] = __coord(outer.width,dim)-1;
                }
            }

            //__________________________________________________________________
            //
            // real indices from outer and inner patch
            //__________________________________________________________________
            void compute_real_indices(const divider<COORD> &full,
                                      const size_t          layers,
                                      const COORD           pbcs) throw()
            {
                //______________________________________________________________
                //
                // get addresses
                //______________________________________________________________
                int *minIndices = &realIndices.imin.x;
                int *maxIndices = &realIndices.imax.x;

                for(size_t dim=0;dim<DIM;++dim)
                {
                    minIndices[dim] = 0;
                    maxIndices[dim] = __coord(outer.width,dim)-1;
                }

                //______________________________________________________________
                //
                // update real_indices
                //______________________________________________________________
                for(size_t dim=0;dim<DIM;++dim)
                {

                    const coord1D dim_rank     = __coord(ranks,dim);
                    const coord1D dim_last     = __coord(full.lasts,dim);
                    const bool    is_dim_first = (0       ==dim_rank);
                    const bool    is_dim_last  = (dim_last==dim_rank);
                    int &         imin         = minIndices[dim];
                    int &         imax         = maxIndices[dim];
                    const coord1D outer_lo     = __coord(outer.lower,dim);
                    const coord1D outer_up     = __coord(outer.upper,dim);
                    const coord1D inner_lo     = __coord(inner.lower,dim);
                    const coord1D inner_up     = __coord(inner.upper,dim);

                    //__________________________________________________________
                    //
                    // remove lowest swaps in any case
                    //__________________________________________________________
                    bool removed_lo = false;
                    bool removed_up = false;
                    
                    if(is_dim_first)
                    {
                        if(outer_lo<inner_lo)
                        {
                            imin += layers;
                            removed_lo = true;
                        }
                    }

                    //__________________________________________________________
                    //
                    // remove uppest swaps in any case
                    //__________________________________________________________

                    if(is_dim_last)
                    {
                        if(inner_up<outer_up)
                        {
                            imax -= layers;
                            removed_up = true;
                        }
                    }


                    //__________________________________________________________
                    //
                    // remove all lower ghost
                    //__________________________________________________________
                    if( (!removed_lo) && (outer_lo<inner_lo) )
                    {
                        imin += layers;
                    }


                    if( (!removed_up) && (inner_up<outer_up) )
                    {
                        imax -= layers;
                        imax += 1;
                    }

                }

            }
            
            inline void compute_flags() throw()
            {
                unsigned &f = (unsigned &)flags;
                unsigned  p = 0x01;
                for(size_t dim=0;dim<DIM;++dim)
                {
                    if(__coord(outer.lower,dim)<__coord(inner.lower,dim))
                    {
                        f |= p;
                    }
                    (p <<= 1);
                    if(__coord(outer.upper,dim)>__coord(inner.upper,dim))
                    {
                        f |= p;
                    }
                    (p <<= 1);
                }
            }



        };

        template <typename COORD>
        inline void build_subsets1D_from(const subset<COORD>   &source,
                                         subset<coord1D>::list &subs,
                                         const bool             build)
        {
            const coord1D *inner_lower = (const coord1D *) & source.inner.lower;
            const coord1D *inner_upper = (const coord1D *) & source.inner.upper;
            const coord1D *outer_lower = (const coord1D *) & source.outer.lower;
            const coord1D *outer_upper = (const coord1D *) & source.outer.upper;

            for(size_t dim=0;dim<YOCTO_IPSO_DIM_OF(COORD);++dim)
            {
                //______________________________________________________________
                //
                // take patch by dimension
                //______________________________________________________________
                const patch1D    inner( inner_lower[dim], inner_upper[dim] );
                const patch1D    outer( outer_lower[dim], outer_upper[dim] );
                subset<coord1D> *sub = new subset<coord1D>(source.rank,inner,outer);
                subs.push_back(sub);
                swaps_list &local = (swaps_list &)(sub->local[0]);
                swaps_list &async = (swaps_list &)(sub->async[0]);
                sub->realIndices.imin[1] = source.realIndices.imin[dim+1];
                sub->realIndices.imax[1] = source.realIndices.imax[dim+1];

                //______________________________________________________________
                //
                // duplicate topology and load offsets
                //______________________________________________________________
                for(const swaps *swp = source.local[dim].head;swp;swp=swp->next)
                {
                    local.push_back(swp->clone1D());
                    local.tail->load(inner,outer,build);
                }

                for(const swaps *swp = source.async[dim].head;swp;swp=swp->next)
                {
                    async.push_back(swp->clone1D());
                    async.tail->load(inner,outer,build);
                }

                //______________________________________________________________
                //
                // and finally register all 'meta' swaps
                //______________________________________________________________
                sub->register_all_swaps();
            }
        }



    }
}

#endif

