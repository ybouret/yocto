#include "yocto/fame/types.hpp"

namespace yocto
{
    namespace fame
    {

        template <> const coord1d coord_info<coord1d>::zero = 0;
        template <> const coord2d coord_info<coord2d>::zero(0,0);
        template <> const coord3d coord_info<coord3d>::zero(0,0,0);
        

    }

}
