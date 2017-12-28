#ifndef YOCTO_IPSO_DOMAIN_INCLUDED
#define YOCTO_IPSO_DOMAIN_INCLUDED 1

#include "yocto/ipso/divide.hpp"
#include "yocto/ipso/ghosts.hpp"

namespace yocto
{
    namespace ipso
    {
        
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

            ghosts::list     async[DIM]; //!< async ghosts
            ghosts::list     local[DIM]; //!< local ghosts

            domain          *next;
            domain          *prev;


            inline void query(size_t &numItems,
                              size_t &numAsync,
                              size_t &numLocal) const throw()
            {
                numItems = inner.items;
                numAsync = 0;
                numLocal = 0;
                for(size_t dim=0;dim<DIM;++dim)
                {
                    for(const ghosts *g=async[dim].head;g;g=g->next)
                    {
                        numAsync += g->count;
                    }
                    for(const ghosts *g=local[dim].head;g;g=g->next)
                    {
                        numLocal += g->count;
                    }
                }
            }


            inline virtual ~domain() throw() {}

#define YOCTO_IPSO_LOWER_GHOSTS new ghosts( ghosts::dim2pos(dim,-1), ng, rank, full.prev_rank(ranks,dim) )
#define YOCTO_IPSO_UPPER_GHOSTS new ghosts( ghosts::dim2pos(dim, 1), ng, rank, full.next_rank(ranks,dim) )

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
            prev(0)
            {
                ng = __coord_abs(ng);
                const bool has_ghosts = (ng>0);
                COORD lower = inner.lower;
                COORD upper = inner.upper;
                for(size_t dim=0;dim<DIM;++dim)
                {
                    //__________________________________________________________
                    //
                    // Pass 1: computing outer
                    //__________________________________________________________
                    const bool    periodic = (0!=__coord(pbcs,dim));
                    coord1D      &lo       = __coord(lower,dim);
                    coord1D      &up       = __coord(upper,dim);
                    const coord1D sz       = __coord(full.sizes,dim);
                    const coord1D rk       = __coord(ranks,dim);
                    const coord1D last     = __coord(full.lasts,dim);
                    if(periodic)
                    {
                        ////////////////////////////////////////////////////////
                        //
                        // PERIODIC CODE
                        //
                        ////////////////////////////////////////////////////////
                        lo-=ng;
                        up+=ng;
                        if(sz>1)
                        {
                            // always async
                            //Y_IPSO_CODE(fprintf(stderr,"<%02u>",unsigned(ranks)));
                            if(has_ghosts)
                            {
                                async[dim].push_back( YOCTO_IPSO_LOWER_GHOSTS );
                                async[dim].push_back( YOCTO_IPSO_UPPER_GHOSTS );
                            }
                        }
                        else
                        {
                            // always local
                            assert(1==sz);
                            //Y_IPSO_CODE(fprintf(stderr,"#%02u#",unsigned(ranks)));
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
                        if(sz>1)
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
                std::cerr << "building async" << std::endl;
                for(size_t dim=0;dim<DIM;++dim)
                {
                    for(ghosts *g = async[dim].head; g; g=g->next)
                    {
                        std::cerr << "\tasync " << g->pos2txt(g->pos) << std::endl;
                        g->load(inner,outer,build);
                    }

                    std::cerr << "building local" << std::endl;
                    for(ghosts *g = local[dim].head; g; g=g->next)
                    {
                        std::cerr << "\tlocal " << g->pos2txt(g->pos) << std::endl;
                        g->load(inner,outer,build);
                    }
                }
            }



        private:
            YOCTO_DISABLE_ASSIGN(domain);
        };

    }
}

#endif

