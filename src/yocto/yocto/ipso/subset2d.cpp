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
            assert(layers>0);
            //------------------------------------------------------------------
            // gather information
            //------------------------------------------------------------------
            const coord2D sizes    = full.sizes;
            const coord2D lasts    = full.lasts;

            //const bool2D  periodic = __coord2bool(pbcs);
            //const bool2D  parallel(sizes.x>1,sizes.y>1);
            std::cerr << "-- CROSS_SWAPS 2D from ranks=" << ranks << "@" << rank << std::endl;
            std::cerr << "-- flags=" << swaps::flg2str(flags) << std::endl;


        }

    }
}


