#include "yocto/ipso/subset.hpp"

namespace yocto
{
    namespace ipso
    {
        template <>
        void subset<coord1D>::load_cross_swaps(const divider<coord1D> &,
                                               const size_t            ,
                                               const coord1D           ,
                                               const bool              )
        {
            // nothing to do
        }
        
    }
}


