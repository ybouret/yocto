#ifndef YOCTO_IPSO_SUBSET_INCLUDED
#define YOCTO_IPSO_SUBSET_INCLUDED 1


#include "yocto/ipso/swaps.hpp"
#include "yocto/ipso/divide.hpp"


namespace yocto
{
    namespace ipso
    {
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
            const swaps::list local; //!< local swaps
            const swaps::list async; //!< async swaps
            const unsigned    local_flags;
            const unsigned    async_flags;
            subset           *next;  //!< for subset::list
            subset           *prev;  //!< for subset::list

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
            local_flags(0),
            async_flags(0),
            next(0),
            prev(0)
            {
                //______________________________________________________________
                //
                // checking swaps availability
                //______________________________________________________________
                const bool has_swaps = (layers>0);
                if(!has_swaps) return;

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
                swaps::list & _async = (swaps::list &)async;

                //______________________________________________________________
                //
                // build outer patch and straight swaps
                //______________________________________________________________
//#define         YOCTO_IPSO_LOWER_SWAPS new swaps(rank, full.prev_rank(ranks,dim), layers, swaps::dim2pos(dim,-1) )
//#define         YOCTO_IPSO_UPPER_SWAPS new swaps(rank, full.next_rank(ranks,dim), layers, swaps::dim2pos(dim, 1) )
#define         YOCTO_IPSO_LOWER_SWAPS(KIND) \
do { const unsigned flag = swaps::dim2pos(dim,-1); _##KIND##_flags |= flag; _##KIND.push_back( new swaps(rank, full.prev_rank(ranks,dim), layers, flag) ); } while(false)
#define         YOCTO_IPSO_UPPER_SWAPS(KIND) \
do { const unsigned flag = swaps::dim2pos(dim, 1); _##KIND##_flags |= flag; _##KIND.push_back( new swaps(rank, full.next_rank(ranks,dim), layers, flag) ); } while(false)


                COORD lower = inner.lower;
                COORD upper = inner.upper;
                unsigned & _local_flags = (unsigned &)local_flags;
                unsigned & _async_flags = (unsigned &)async_flags;
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
                            // SEQUENTIAL in that dimension: always local
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
            }


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(subset);
            void load_cross_swaps(const divider<COORD> &full,
                                  const size_t          layers,
                                  const COORD           pbcs,
                                  const bool            build);
        };


        ////////////////////////////////////////////////////////////////////////
        //
        // automatic list of subsets
        //
        ////////////////////////////////////////////////////////////////////////
        template <typename COORD>
        class subsets : public subset<COORD>::list
        {
        public:
            inline virtual ~subsets() throw() {}

            const COORD sizes;
            inline explicit subsets(const divider<COORD> &full,
                                    const size_t          layers,
                                    const COORD           pbcs,
                                    const bool            build = false ) :
            subset<COORD>::list(),
            sizes( full.sizes )
            {
                for(size_t rank=0;rank<full.size;++rank)
                {
                    this->push_back( new subset<COORD>(full,rank,layers,pbcs,build) );
                }
            }


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(subsets);
        };
    }
}

#endif

