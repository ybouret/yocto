#include "yocto/spade/types.hpp"

namespace yocto
{
    namespace spade
    {
        const char * coord_data::axis_name(const size_t dim) throw()
        {
            static const char *the_names[4] =
            {
                "X",
                "Y",
                "Z",
                ""
            };
            return the_names[dim&3];
        }
        
        template <>
        const coord1D coord_info<coord1D>::zero = 0;
        
        template <>
        const coord2D coord_info<coord2D>::zero(0,0);
        
        template <>
        const coord3D coord_info<coord3D>::zero(0,0,0);
    }
}
