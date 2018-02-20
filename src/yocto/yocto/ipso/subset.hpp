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

            const COORD      ranks;
            const patch_type inner;
            const patch_type outer;
            swaps::list      local; //!< local swaps
            swaps::list      async; //!< async swaps
            subset          *next;  //!< for subset::list
            subset          *prev;  //!< for subset::list

            inline virtual ~subset() throw() {}

            //! build the outer layout
            inline explicit subset(const divider<COORD> &full,
                                   const size_t          rank,
                                   const size_t          layers,
                                   const COORD           pbcs,
                                   const bool            build) :
            ranks(),
            inner( full(rank, (COORD *)&ranks) ),
            outer( inner ),
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
                    const coord1D ww = __coord(inner.width,dim);
                    if(layers>ww)
                    {
                        throw libc::exception(EDOM,"subset%uD: layers=%d>width.%s=%d", unsigned(DIM), int(layers), __coord_name(dim),int(ww));
                    }
                }

                //______________________________________________________________
                //
                // build outer patch and order 0 ghosts
                //______________________________________________________________
#define         YOCTO_IPSO_LOWER_SWAPS new swaps(rank, full.prev_rank(ranks,dim), layers, swaps::dim2pos(dim,-1) )
#define         YOCTO_IPSO_UPPER_SWAPS new swaps(rank, full.next_rank(ranks,dim), layers, swaps::dim2pos(dim, 1) )

                COORD lower = inner.lower;
                COORD upper = inner.upper;
                for(size_t dim=0;dim<DIM;++dim)
                {
                    //assert(0==local[dim].size);
                    //assert(0==async[dim].size);
                    //__________________________________________________________
                    //
                    // Pass 1: computing outer
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
                                async.push_back( YOCTO_IPSO_LOWER_SWAPS );
                                async.push_back( YOCTO_IPSO_UPPER_SWAPS );
                            }
                        }
                        else
                        {
                            //__________________________________________________
                            //
                            // SEQUENTIAL in that dimension: always local
                            //__________________________________________________
                            assert(1==sz);
                            if(has_swaps)
                            {
                                local.push_back( YOCTO_IPSO_LOWER_SWAPS );
                                local.push_back( YOCTO_IPSO_UPPER_SWAPS );
                            }
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
                                if(has_swaps)
                                {
                                    async.push_back( YOCTO_IPSO_UPPER_SWAPS );
                                }
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
                                    if(has_swaps)
                                    {
                                        async.push_back( YOCTO_IPSO_LOWER_SWAPS );
                                    }
                                }
                                else
                                {
                                    // in bulk
                                    lo -= layers;
                                    up += layers;
                                    if(has_swaps)
                                    {
                                        async.push_back( YOCTO_IPSO_LOWER_SWAPS );
                                        async.push_back( YOCTO_IPSO_UPPER_SWAPS );
                                    }
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

                for(swaps *swp = async.tail; swp; swp=swp->prev )
                {
                    swp->load(inner,outer,build);
                }

                for(swaps *swp = local.tail; swp; swp=swp->prev )
                {
                    swp->load(inner,outer,build);
                }
            }


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(subset);
            void setup(const divider<COORD> &full,
                       const size_t          rank,
                       const size_t          layers,
                       const COORD           pbcs);
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

