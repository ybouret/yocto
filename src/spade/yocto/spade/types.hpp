#ifndef YOCTO_SPADE_TYPES_INCLUDED
#define YOCTO_SPADE_TYPES_INCLUDED

#include "yocto/math/point2d.hpp"
#include "yocto/math/point3d.hpp"
#include "yocto/math/types.hpp"

namespace yocto
{
    namespace spade
    {
        typedef unit_t           coord1D;
        typedef point2d<coord1D> coord2D;
        typedef point3d<coord1D> coord3D;

        template <typename COORD>
        struct coord_info
        {
            static const COORD zero;
        };
        
        //! extract coord from compound COORD
        template <typename COORD>
        inline unit_t & __coord( COORD &C, size_t dim ) throw()
        {
            assert(dim<sizeof(COORD)/sizeof(unit_t));
            return *(((unit_t *)&C)+dim);
        }

        //! extract coord from compound COORD (const)
        template <typename COORD>
        inline const unit_t & __coord( const COORD &C, size_t dim ) throw()
        {
            assert(dim<sizeof(COORD)/sizeof(unit_t));
            return *(((unit_t *)&C)+dim);
        }

        //! decrease all coordinates
        template <typename COORD>
        void __coord_dec(COORD &C) throw();

        template <> inline void __coord_dec<coord1D>(coord1D &C) throw() { --C;                 }
        template <> inline void __coord_dec<coord2D>(coord2D &C) throw() { --C.x; --C.y;        }
        template <> inline void __coord_dec<coord3D>(coord3D &C) throw() { --C.x; --C.y; --C.z; }

        //! keep positive par only
        template <typename COORD>
        COORD __coord_ge_zero(const COORD &C) throw();
        
        template <> inline coord1D __coord_ge_zero<coord1D>(const coord1D &C) throw() { return (C<0) ? 0 : C; }
        template <> inline coord2D __coord_ge_zero<coord2D>(const coord2D &C) throw()
        {
            return coord2D( __coord_ge_zero(C.x), __coord_ge_zero(C.y) );
        }
        template <> inline coord3D __coord_ge_zero<coord3D>(const coord3D &C) throw()
        {
            return coord3D( __coord_ge_zero(C.x), __coord_ge_zero(C.y), __coord_ge_zero(C.z) );
        }


        //! test equality
        template <typename COORD>
        bool are_same_coord(const COORD &lhs, const COORD &rhs) throw();

        template <> inline bool are_same_coord<coord1D>(const coord1D &lhs, const coord1D &rhs) throw()
        { return lhs == rhs; }

        template <> inline bool are_same_coord<coord2D>(const coord2D &lhs, const coord2D &rhs) throw()
        { return (lhs.x == rhs.x) && (lhs.y==rhs.y); }

        template <> inline bool are_same_coord<coord3D>(const coord3D &lhs, const coord3D &rhs) throw()
        { return (lhs.x == rhs.x) && (lhs.y==rhs.y) && (lhs.z==rhs.z); }

        //! test within
        template <typename COORD>
        bool is_coord_inside(const COORD &C, const COORD &lower, const COORD &upper) throw();

        template <> inline bool is_coord_inside<coord1D>(const coord1D &C, const coord1D &lower, const coord1D &upper) throw()
        {
            assert(lower<=upper);
            return (C>=lower) && (C<=upper);
        }

        template <> inline bool is_coord_inside<coord2D>(const coord2D &C, const coord2D &lower, const coord2D &upper) throw()
        {
            assert(lower.x<=upper.x);
            assert(lower.y<=upper.y);
            return ((C.x>=lower.x) && (C.x<=upper.x)) && ((C.y>=lower.y) && (C.y<=upper.y));
        }

        template <> inline bool is_coord_inside<coord3D>(const coord3D &C, const coord3D &lower, const coord3D &upper) throw()
        {
            assert(lower.x<=upper.x);
            assert(lower.y<=upper.y);
            assert(lower.z<=upper.z);
            return ((C.x>=lower.x) && (C.x<=upper.x)) && ((C.y>=lower.y) && (C.y<=upper.y)) && ((C.z>=lower.z) && (C.z<=upper.z));
        }



    }
}


#endif
