#ifndef YOCTO_FAME_TYPES_INCLUDED
#define YOCTO_FAME_TYPES_INCLUDED 1


#include "yocto/math/point3d.hpp"
#include "yocto/type/traits.hpp"

namespace yocto
{
    namespace fame
    {
        typedef unit_t           coord1d;
        typedef point2d<coord1d> coord2d;
        typedef point3d<coord1d> coord3d;
        
#define YOCTO_SPADE_DECL_COORD \
typedef COORD                                         coord;         \
typedef const     COORD                               const_coord;   \
typedef typename  type_traits<COORD>::parameter_type  param_coord;
        
        template <typename COORD>
        struct coord_info
        {
            YOCTO_SPADE_DECL_COORD;
            static const size_t DIMENSION = sizeof(COORD)/sizeof(coord1d);
            static const_coord  zero;
        };
        
        template <typename COORD>
        inline coord1d & __coord(COORD &C,const size_t dim) throw()
        {
            assert(dim<coord_info<COORD>::DIMENSION);
            return *(((coord1d *)&C)+dim);
        }
        
        template <typename COORD>
        inline const coord1d & __coord(const COORD &C,const size_t dim) throw()
        {
            assert(dim<coord_info<COORD>::DIMENSION);
            return *(((const coord1d *)&C)+dim);
        }


    }
    
}

#endif
