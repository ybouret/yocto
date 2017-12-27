#include "yocto/ipso/domain.hpp"

#define Y_IPSO_SHOW 1
#if defined(Y_IPSO_SHOW) && (1==Y_IPSO_SHOW)
#include <cstdio>
#define Y_IPSO_CODE(CODE) CODE
#else
#define Y_IPSO_CODE(CODE)
#endif

namespace yocto
{
    namespace ipso
    {



        template <>
        domain<coord1D>::domain(const divider<coord1D> &full,
                                const size_t            rank,
                                coord1D                 ng,
                                const coord1D           pbcs) :
        ranks( 0 ),
        inner( full(rank, (coord1D *)&ranks) ),
        outer( inner ),
        async(),
        local(),
        next(0),
        prev(0)
        {
            ng = __coord_abs(ng);
            const bool has_ghosts = (ng>0);
            coord1D lo = inner.lower;
            coord1D up = inner.upper;

            //__________________________________________________________________
            //
            // Pass 1: computing outer
            //__________________________________________________________________

            const bool periodic = (0!=pbcs);
            if(periodic)
            {
                ////////////////////////////////////////////////////////////////
                //
                // PERIODIC CODE
                //
                ////////////////////////////////////////////////////////////////
                lo-=ng;
                up+=ng;
                if(full.sizes>1)
                {
                    // always async
                    Y_IPSO_CODE(fprintf(stderr,"<%02u>",unsigned(ranks)));
                    if(has_ghosts)
                    {
                        async.push_back( new ghosts(-1,ng) );
                        async.push_back( new ghosts(1,ng) );
                    }
                }
                else
                {
                    // always local
                    assert(1==full.sizes);
                    Y_IPSO_CODE(fprintf(stderr,"#%02u#",unsigned(ranks)));
                    if(has_ghosts)
                    {
                        local.push_back( new ghosts(-1,ng) );
                        local.push_back( new ghosts(1,ng) );
                    }
                }
            }
            else
            {
                ////////////////////////////////////////////////////////////////
                //
                // NOT PERIODIC CODE
                //
                ////////////////////////////////////////////////////////////////
                if(full.sizes>1)
                {
                    if(0==ranks)
                    {
                        // @left
                        up += ng;
                        Y_IPSO_CODE(fprintf(stderr,"|%02u>",unsigned(ranks)));
                        if(has_ghosts)
                        {
                            async.push_back( new ghosts(1,ng) );
                        }
                    }
                    else
                    {
                        if(full.lasts==ranks)
                        {
                            // @right
                            lo -= ng;
                            Y_IPSO_CODE(fprintf(stderr,"<%02u|",unsigned(ranks)));
                            if(has_ghosts)
                            {
                                async.push_back( new ghosts(-1,ng) );
                            }
                        }
                        else
                        {
                            // in bulk
                            lo -= ng;
                            up += ng;
                            Y_IPSO_CODE(fprintf(stderr,"<%02u>",unsigned(ranks)));
                            if(has_ghosts)
                            {
                                async.push_back( new ghosts(-1,ng) );
                                async.push_back( new ghosts(1,ng)  );
                            }
                        }
                    }
                }
                else
                {
                    // nothing to do
                    Y_IPSO_CODE(fprintf(stderr,"|%02u|",unsigned(ranks)));
                }
            }

            new ((void*)&outer) patch1D(lo,up);


            //__________________________________________________________________
            //
            // Pass 2: loading ghosts
            //__________________________________________________________________
            for(ghosts *g = async.head; g; g=g->next)
            {
                g->load(inner,outer);
            }
            
            for(ghosts *g = local.head; g; g=g->next)
            {
                g->load(inner,outer);
            }

        }


    }

}

