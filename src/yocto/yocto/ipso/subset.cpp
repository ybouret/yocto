#include "yocto/ipso/subset.hpp"

namespace yocto
{
    namespace ipso
    {
        template <>
        void subset<coord1D>::setup(const divider<coord1D> &full,
                                    const size_t            rank,
                                    const size_t            layers,
                                    const coord1D           pbcs)
        {
            assert(layers>0);
#if 0
            const bool    periodic = (pbcs!=0);
            const coord1D sz       = full.sizes;
            const coord1D rk       = ranks;
            const bool    parallel = (sz>1);
            const coord1D last     = full.lasts;

            coord1D       lower    = inner.lower;
            coord1D       upper    = inner.upper;

            if(periodic)
            {
                ////////////////////////////////////////////////////////////////
                //
                // PERIODIC 
                //
                ////////////////////////////////////////////////////////////////
                lower -= layers;
                upper += layers;
                if(parallel)
                {
                    ////////////////////////////////////////////////////////////
                    // two async ghosts along x
                    ////////////////////////////////////////////////////////////
                    async.push_back( new swaps(rank, full.prev_rank(ranks,0),layers,swaps::lower_x ) );
                    async.push_back( new swaps(rank, full.next_rank(ranks,0),layers,swaps::upper_x ) );
                }
                else
                {
                    ////////////////////////////////////////////////////////////
                    // two local ghosts along x
                    ////////////////////////////////////////////////////////////
                    local.push_back( new swaps(rank, full.prev_rank(ranks,0),layers,swaps::lower_x ) );
                    local.push_back( new swaps(rank, full.next_rank(ranks,0),layers,swaps::upper_x ) );
                }
            }
            else
            {
                ////////////////////////////////////////////////////////////////
                //
                // NOT PERIODIC
                //
                ////////////////////////////////////////////////////////////////
            }
#endif
        }
    }
}


