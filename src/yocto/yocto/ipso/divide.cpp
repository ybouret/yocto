#include "yocto/ipso/divide.hpp"

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
        void divide:: metrics<coord1D>:: compute(const __divide<coord1D> &full,
                                                 const size_t             rank,
                                                 const coord1D            pbcs)
        {
            // get the current sub patch
            coord1D       ranks = 0;
            const patch1D sub   = full(rank,&ranks);

            // get the work load
            load = sub.items;

            const bool periodic = (0!=pbcs);

            if(periodic)
            {
                // PERIODIC
                if(full.sizes>1)
                {
                    coms += 2;
                    Y_IPSO_CODE(fprintf(stderr,"+%02u+",unsigned(ranks)));
                }
                else
                {
                    copy += 2;
                    Y_IPSO_CODE(fprintf(stderr,"#%02u#",unsigned(ranks)));
                }
            }
            else
            {
                // NOT PERIODIC
                if(full.sizes>1)
                {
                    if(0==ranks)
                    {
                        //@left
                        coms += 1;
                        Y_IPSO_CODE(fprintf(stderr,"|%02u+",unsigned(ranks)));
                    }
                    else
                    {
                        if(full.lasts==ranks)
                        {
                            //@right
                            coms += 1;
                            Y_IPSO_CODE(fprintf(stderr,"+%02u|",unsigned(ranks)));
                        }
                        else
                        {
                            //bulk
                            coms += 2;
                            Y_IPSO_CODE(fprintf(stderr,"+%02u+",unsigned(ranks)));
                        }
                    }
                }
                else
                {
                    // nothing to do!
                    assert(0==ranks);
                    Y_IPSO_CODE(fprintf(stderr,"|%02u|",unsigned(ranks)));
                }
            }





        }
    }
}




