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
            const coord1D width    = layers;
            const coord1D shift    = width-1;
            const coord2D delta(shift,shift);

            //const bool2D  periodic = __coord2bool(pbcs);
            //const bool2D  parallel(sizes.x>1,sizes.y>1);
            std::cerr << "-- CROSS_SWAPS 2D from ranks=" << ranks << "@" << rank << std::endl;
            std::cerr << "-- flags=" << swaps::flg2str(flags) << std::endl;

            //------------------------------------------------------------------
            // going clockwise
            //------------------------------------------------------------------
            if( flags & swaps::xup_yup )
            {
                std::cerr << "\t@xup_yup" << std::endl;
                const coord2D inner_start = inner.upper - delta;
                const coord2D outer_start = inner.upper + coord2D(1,1);
                //swaps(rank,target,layers,swaps::xup_yup);

            }

        }

    }
}


