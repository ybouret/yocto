#include "yocto/ipso/subset.hpp"

namespace yocto
{
    namespace ipso
    {
        template <>
        void subset<coord2D>::load_cross_swaps(const divider<coord2D> &full,
                                               const size_t            layers,
                                               const coord2D           pbcs,
                                               const bool              build)
        {
            //------------------------------------------------------------------
            // gather information
            //------------------------------------------------------------------
            const bool2D  periodic = __coord2bool(pbcs);
            const coord2D sizes    = full.sizes;
            const coord2D lasts    = full.lasts;
            //unsigned      flag     = 0;


        }

    }
}


