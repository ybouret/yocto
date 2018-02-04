#ifndef YOCTO_IPSO_DOMAIN_INCLUDED
#define YOCTO_IPSO_DOMAIN_INCLUDED 1

#include "yocto/ipso/divide.hpp"
#include "yocto/ipso/ghosts.hpp"
#include "yocto/counted-object.hpp"

namespace yocto
{
    namespace ipso
    {

        //! a domain describe the full topolgy of a field
        template <typename COORD>
        class domain : public object
        {
        public:
            typedef core::list_of_cpp<domain> list;
            typedef patch<COORD>              patch_type;
            static const size_t DIM = sizeof(COORD)/sizeof(coord1D);

            const COORD      ranks;
            const patch_type inner;
            const patch_type outer;

            ghosts::list     async[DIM]; //!< async ghosts, by dimension: 0, 1 or 2
            ghosts::list     local[DIM]; //!< local ghosts, by dimension: 0 or 2
            domain          *next;       //!< for domain::list
            domain          *prev;       //!< for domain::list
            const COORD      asyncs;     //!< max async by dimension

            inline virtual ~domain() throw() {}

#define YOCTO_IPSO_LOWER_GHOSTS new ghosts( ghosts::dim2pos(dim,-1), ng, rank, full.prev_rank(ranks,dim) )
#define YOCTO_IPSO_UPPER_GHOSTS new ghosts( ghosts::dim2pos(dim, 1), ng, rank, full.next_rank(ranks,dim) )

            //! build the outer layout and ghosts
            /**
             now that's one function...
             \param full a prepared divider with its size and sizes
             \param rank the domain rank, rank<divider.size
             \param ng   the number of desired ghosts
             \param pbcs the periodic boundary conditions in each dimension
             \param build if true, then compute the ghost's indices, not just the count
             */
            inline explicit domain(const divider<COORD> &full,
                                   const size_t          rank,
                                   coord1D               ng,
                                   const COORD           pbcs,
                                   const bool            build) :
            ranks(),
            inner( full(rank, (COORD *)&ranks) ),
            outer( inner ),
            async(),
            local(),
            next(0),
            prev(0),
            asyncs()
            {
                //______________________________________________________________
                //
                // checking ghosts availability
                //______________________________________________________________
                ng = abs_of(ng);
                const bool has_ghosts = (ng>0);
                if(has_ghosts)
                {
                    for(size_t dim=0;dim<DIM;++dim)
                    {
                        const coord1D ww = __coord(inner.width,dim);
                        if(ng>ww)
                        {
                            throw libc::exception(EDOM,"domain%uD: #ghost=%d>width.%s=%d", unsigned(DIM), int(ng), __coord_name(dim),int(ww));
                        }
                    }
                }

                //______________________________________________________________
                //
                // building domain
                //______________________________________________________________
                COORD lower = inner.lower;
                COORD upper = inner.upper;
                for(size_t dim=0;dim<DIM;++dim)
                {
                    assert(0==local[dim].size);
                    assert(0==async[dim].size);
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
                        lo-=ng;
                        up+=ng;
                        if(parallel)
                        {
                            //__________________________________________________
                            //
                            // PARALLEL in that dimension: always async
                            //__________________________________________________
                            if(has_ghosts)
                            {
                                async[dim].push_back( YOCTO_IPSO_LOWER_GHOSTS );
                                async[dim].push_back( YOCTO_IPSO_UPPER_GHOSTS );
                            }
                        }
                        else
                        {
                            //__________________________________________________
                            //
                            // SEQUENTIAL in that dimension: always local
                            //__________________________________________________
                            assert(1==sz);
                            if(has_ghosts)
                            {
                                local[dim].push_back( YOCTO_IPSO_LOWER_GHOSTS );
                                local[dim].push_back( YOCTO_IPSO_UPPER_GHOSTS );
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
                                // not periodic @first
                                up += ng;
                                //Y_IPSO_CODE(fprintf(stderr,"|%02u>",unsigned(ranks)));
                                if(has_ghosts)
                                {
                                    async[dim].push_back( YOCTO_IPSO_UPPER_GHOSTS );
                                }
                            }
                            else
                            {
                                if(last==rk)
                                {
                                    // not periodic @last
                                    lo -= ng;
                                    //Y_IPSO_CODE(fprintf(stderr,"<%02u|",unsigned(ranks)));
                                    if(has_ghosts)
                                    {
                                        async[dim].push_back( YOCTO_IPSO_LOWER_GHOSTS );
                                    }
                                }
                                else
                                {
                                    // in bulk
                                    lo -= ng;
                                    up += ng;
                                    //Y_IPSO_CODE(fprintf(stderr,"<%02u>",unsigned(ranks)));
                                    if(has_ghosts)
                                    {
                                        async[dim].push_back( YOCTO_IPSO_LOWER_GHOSTS );
                                        async[dim].push_back( YOCTO_IPSO_UPPER_GHOSTS );
                                    }
                                }
                            }
                        }
                        else
                        {
                            // nothing to do
                            //Y_IPSO_CODE(fprintf(stderr,"|%02u|",unsigned(ranks)));
                        }
                    }

                }
                new ((void*)&outer) patch_type(lower,upper);


                //______________________________________________________________
                //
                // Pass 2: loading ghosts
                //______________________________________________________________
                size_t       num_async  = 0;
                size_t       num_local  = 0;
                for(size_t dim=0;dim<DIM;++dim)
                {
                    coord1D &max_async = __coord( (COORD &)asyncs, dim );
                    max_async = 0;
                    for(ghosts *g = async[dim].head; g; g=g->next)
                    {
                        assert(rank==g->source);
                        g->load(inner,outer,build);
                        num_async += g->count;
                        if(g->count>max_async)
                        {
                            max_async = g->count;
                        }
                    }

                    for(ghosts *g = local[dim].head; g; g=g->next)
                    {
                        g->load(inner,outer,build);
                        num_local += g->count;
                    }
                }
                
            }


            //! copy the local ghosts in their position
            /**
             \param F a field based on the outer layout
             */
            template <typename FIELD>
            inline void copyLocal( FIELD &F ) const throw()
            {
                assert( patch_type::eq(outer,F) );
                for(size_t dim=0;dim<DIM;++dim)
                {
                    const ghosts::list &G = local[dim];
                    if(G.size>0)
                    {
                        std::cerr << "copyLocal[" << dim << "] #=" << G.head->count << std::endl;
                    }
                    F.local_exchange(G);
                }
            }


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(domain);
        };

        typedef domain<coord1D> domain1D;
        typedef domain<coord2D> domain2D;
        typedef domain<coord3D> domain3D;
    }
}

#endif

