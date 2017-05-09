#ifndef YOCTO_SPADE_TYPES_INCLUDED
#define YOCTO_SPADE_TYPES_INCLUDED

#include "yocto/math/point2d.hpp"
#include "yocto/math/point3d.hpp"
#include "yocto/math/types.hpp"
#include "yocto/code/utils.hpp"

namespace yocto
{
    namespace spade
    {
        typedef unit_t           coord1D; //!< alias for signed coordinate
        typedef point2d<coord1D> coord2D; //!< in 2D
        typedef point3d<coord1D> coord3D; //!< in 3D

        struct coord_data
        {
            static const char *axis_name(const size_t dim) throw();
        };

        //! define a zero coordinate
        template <typename COORD>
        struct coord_info
        {
            static const COORD zero;
        };

        //! extract coord from compound COORD
        template <typename COORD>
        inline unit_t & __coord( COORD &C, size_t dim ) throw()
        {
            assert(dim<sizeof(COORD)/sizeof(coord1D));
            return *(((unit_t *)&C)+dim);
        }

        //! extract coord from compound COORD (const)
        template <typename COORD>
        inline const unit_t & __coord( const COORD &C, size_t dim ) throw()
        {
            assert(dim<sizeof(COORD)/sizeof(coord1D));
            return *(((unit_t *)&C)+dim);
        }

        //! product of coordinates
        template <typename COORD>
        inline coord1D __coord_prod( const COORD &C) throw();

        template <> inline coord1D __coord_prod<coord1D>(const coord1D &C) throw() { return C;           }
        template <> inline coord1D __coord_prod<coord2D>(const coord2D &C) throw() { return C.x*C.y;     }
        template <> inline coord1D __coord_prod<coord3D>(const coord3D &C) throw() { return C.x*C.y*C.z; }


        //! inline lower
        template <typename COORD>
        inline COORD __coord_lower_of(const COORD &lhs, const COORD &rhs) throw()
        {
            COORD          ans( coord_info<COORD>::zero );
            coord1D       *p = (coord1D *) &ans;
            const coord1D *l = (const coord1D *) &lhs;
            const coord1D *r = (const coord1D *) &rhs;
            for(size_t dim=0;dim<sizeof(COORD)/sizeof(coord1D);++dim)
            {
                p[dim] = min_of(l[dim],r[dim]);
            }
            return ans;
        }


        //! inline upper
        template <typename COORD>
        inline COORD __coord_upper_of(const COORD &lhs, const COORD &rhs) throw()
        {
            COORD          ans( coord_info<COORD>::zero );
            coord1D       *p = (coord1D *) &ans;
            const coord1D *l = (const coord1D *) &lhs;
            const coord1D *r = (const coord1D *) &rhs;
            for(size_t dim=0;dim<sizeof(COORD)/sizeof(coord1D);++dim)
            {
                p[dim] = max_of(l[dim],r[dim]);
            }
            return ans;
        }




        //! decrease all coordinates
        template <typename COORD>
        void __coord_dec(COORD &C) throw();

        template <> inline void __coord_dec<coord1D>(coord1D &C) throw() { --C;                 }
        template <> inline void __coord_dec<coord2D>(coord2D &C) throw() { --C.x; --C.y;        }
        template <> inline void __coord_dec<coord3D>(coord3D &C) throw() { --C.x; --C.y; --C.z; }

        //! keep positive par only
        template <typename COORD>
        COORD __coord_abs(const COORD &C) throw();
        
        template <> inline coord1D __coord_abs<coord1D>(const coord1D &C) throw() { return (C<0) ? -C : C; }
        template <> inline coord2D __coord_abs<coord2D>(const coord2D &C) throw()
        {
            return coord2D( __coord_abs(C.x), __coord_abs(C.y) );
        }
        template <> inline coord3D __coord_abs<coord3D>(const coord3D &C) throw()
        {
            return coord3D( __coord_abs(C.x), __coord_abs(C.y), __coord_abs(C.z) );
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

#define YOCTO_SPADE_DECL_COORD \
typedef COORD                                         coord;         \
typedef const     COORD                               const_coord;   \
typedef typename  type_traits<COORD>::parameter_type  param_coord;

        //______________________________________________________________________
        //
        //
        //
        //______________________________________________________________________

        template <typename T,typename COORD>
        struct vertex_for;

        template <typename T>
        struct vertex_for<T,coord1D>
        {
            typedef T type;
        };

        template <typename T>
        struct vertex_for<T,coord2D>
        {
            typedef point2d<T> type;
        };

        template <typename T>
        struct vertex_for<T,coord3D>
        {
            typedef point3d<T> type;
        };

        //______________________________________________________________________
        //
        //
        //
        //______________________________________________________________________

        template <typename T,typename COORD>
        class box_of
        {
        public:
            static const size_t DIMENSION = sizeof(COORD)/sizeof(coord1D);
            YOCTO_ARGUMENTS_DECL_T;
            YOCTO_SPADE_DECL_COORD;

            typedef typename vertex_for<T,COORD>::type        vtx;
            typedef typename type_traits<vtx>::parameter_type param_vtx;
            typedef          const vtx                        const_vtx;


            const_vtx  lower;
            const_vtx  upper;
            const_vtx  width;
            const_type volume;

            inline virtual ~box_of() throw() {}
            inline explicit box_of( param_vtx lo, param_vtx up ) throw() :
            lower(lo),
            upper(up),
            width(lo),//dummy
            volume(1)
            {
                type *L = (type *)&lower;
                type *U = (type *)&upper;
                type *W = (type *)&width;
                type &V = (type &)volume;
                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                    if(U[dim]<L[dim]) cswap(U[dim],L[dim]);
                    assert(L[dim]<=U[dim]);
                    V *= (W[dim]=U[dim]-L[dim]);
                }
            }

            inline box_of( const box_of &other ) throw() :
            lower(other.lower),
            upper(other.upper),
            volume(other.volume)
            {
            }
            

            inline const_type *__lower() const throw() { return (const_type *) &lower; }
            inline const_type *__upper() const throw() { return (const_type *) &upper; }
            inline const_type *__width() const throw() { return (const_type *) &width; }
            


            inline friend std::ostream & operator<<( std::ostream &os, const box_of &p )
            {
                os << "| " << p.lower << " -> " << p.upper << ", volume=" << p.volume << " |";
                return os;
            }


        private:
            YOCTO_DISABLE_ASSIGN(box_of);
        };


    }
}


#endif
