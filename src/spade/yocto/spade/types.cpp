#include "yocto/spade/types.hpp"

namespace yocto
{
    namespace spade
    {
        
        
        template <>
        const coord1D coord_info<coord1D>::zero = 0;
        
        template <>
        const coord2D coord_info<coord2D>::zero;
        
        template <>
        const coord3D coord_info<coord3D>::zero;
    }
}
