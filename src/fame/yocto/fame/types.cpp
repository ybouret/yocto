#include "yocto/fame/types.hpp"

namespace yocto
{
    namespace fame
    {

        template <> const coord1d coord_data<coord1d>::zero = 0;
        template <> const coord2d coord_data<coord2d>::zero = coord2d(0,0);
        template <> const coord3d coord_data<coord3d>::zero = coord3d(0,0,0);
        
        const char * coord_info::axis_name(const size_t dim) throw()
        {
            switch(dim%3)
            {
                case 0: return "x";
                case 1: return "y";
                case 2: return "z";
            }
            return "";
        }
    }
}
