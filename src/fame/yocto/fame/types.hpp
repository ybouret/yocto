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
        
#define YOCTO_FAME_DIM_OF(COORD) ( sizeof(COORD)/sizeof(coord1d) )
        
#define YOCTO_FAME_PARAM(COORD)  typename type_traits<COORD>::parameter_type
        
#define YOCTO_FAME_DECL_COORD                                        \
typedef COORD                                         coord;         \
typedef const     COORD                               const_coord;   \
typedef YOCTO_FAME_PARAM(COORD)                       param_coord;   \
static const size_t DIMENSION = YOCTO_FAME_DIM_OF(COORD)
        
        template <typename COORD>
        struct coord_info
        {
            YOCTO_FAME_DECL_COORD;
            static const_coord  zero;
        };
        
        template <typename COORD>
        inline coord1d *__coord_addr(const COORD &C) throw()
        {
            return (coord1d *)&C;
        }
        
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

        //! product of coordinates
        template <typename COORD>
        inline coord1d __coord_prod(const COORD &C) throw()
        {
            const coord1d *q = __coord_addr(C);
            coord1d        p = q[0];
            for(size_t dim=1;dim<YOCTO_FAME_DIM_OF(COORD);++dim)
            {
                p *= q[dim];
            }
            return p;
        }
        
        //! test for integral types
        template <typename COORD> inline
        bool are_same_coord(const YOCTO_FAME_PARAM(COORD) lhs,
                            const YOCTO_FAME_PARAM(COORD) rhs) throw()
        {
            const coord1d *l = __coord_addr(lhs);
            const coord1d *r = __coord_addr(rhs);
            for(size_t i=0;i<YOCTO_FAME_DIM_OF(COORD);++i)
            {
                if( l[i]!=r[i] ) return false;
            }
            return true;
        }
        
        //!
        template <typename COORD> inline
        bool is_coord_inside(const YOCTO_FAME_PARAM(COORD) C,
                             const YOCTO_FAME_PARAM(COORD) lower,
                             const YOCTO_FAME_PARAM(COORD) upper) throw()
        {
            const coord1d *l = __coord_addr(lower);
            const coord1d *u = __coord_addr(upper);
            const coord1d *c = __coord_addr(C);
            for(size_t i=0;i<YOCTO_FAME_DIM_OF(COORD);++i)
            {
                assert(l[i]<=u[i]);
                if( c[i]<l[i] || c[i]>u[i]) return false;
            }
            return true;
        }
        
    }
    
}

#endif
