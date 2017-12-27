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
                                const coord1D           ng,
                                const coord1D           pbcs) :
        ranks( 0 ),
        inner( full(rank, (coord1D *)&ranks) ),
        outer( inner )
        {
            assert(ng>=0);
            coord1D lo = inner.lower;
            coord1D up = inner.upper;

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
                }
                else
                {
                    // 
                    assert(1==full.sizes);
                    Y_IPSO_CODE(fprintf(stderr,"#%02u#",unsigned(ranks)));
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
                    }
                    else
                    {
                        if(full.lasts==ranks)
                        {
                            // @right
                            lo -= ng;
                            Y_IPSO_CODE(fprintf(stderr,"<%02u|",unsigned(ranks)));
                        }
                        else
                        {
                            // in bulk
                            lo -= ng;
                            up += ng;
                            Y_IPSO_CODE(fprintf(stderr,"<%02u>",unsigned(ranks)));
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

        }


    }

}

