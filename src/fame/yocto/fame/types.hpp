#ifndef YOCTO_FAME_TYPES_INCLUDED
#define YOCTO_FAME_TYPES_INCLUDED 1


#include "yocto/math/point3d.hpp"
#include "yocto/type/traits.hpp"
#include "yocto/hashing/function.hpp"
#include "yocto/code/bswap.hpp"

namespace yocto
{
    namespace fame
    {
        typedef unit_t           coord1d;
        typedef point2d<coord1d> coord2d;
        typedef point3d<coord1d> coord3d;
        
        struct coord_info
        {
            static const char *axis_name(const size_t dim) throw();
        };
        
#define YOCTO_FAME_DIM_OF(COORD) ( sizeof(COORD)/sizeof(coord1d) )
        
#define YOCTO_FAME_PARAM(COORD)  typename type_traits<COORD>::parameter_type
        
#define YOCTO_FAME_DECL_COORD                                        \
typedef COORD                                         coord;         \
typedef const     COORD                               const_coord;   \
typedef YOCTO_FAME_PARAM(COORD)                       param_coord;   \
static const size_t DIMENSION = YOCTO_FAME_DIM_OF(COORD)
        
        template <typename COORD>
        struct coord_data
        {
            YOCTO_FAME_DECL_COORD;
            static const_coord  zero;
        };

#define YOCTO_FAME_COORD_EXTERN_(TYPE) extern template const yocto::fame::TYPE yocto::fame::coord_data<yocto::fame::TYPE>::zero
#define YOCTO_FAME_COORD_EXTERN()  \
YOCTO_FAME_COORD_EXTERN_(coord1d); \
YOCTO_FAME_COORD_EXTERN_(coord2d); \
YOCTO_FAME_COORD_EXTERN_(coord3d)

        template <typename COORD>
        inline coord1d *__coord_addr(const COORD &C) throw()
        {
            return (coord1d *)&C;
        }
        
        template <typename COORD>
        inline coord1d & __coord(COORD &C,const size_t dim) throw()
        {
            assert(dim<coord_data<COORD>::DIMENSION);
            return *(((coord1d *)&C)+dim);
        }
        
        template <typename COORD>
        inline const coord1d & __coord(const COORD &C,const size_t dim) throw()
        {
            assert(dim<coord_data<COORD>::DIMENSION);
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
        
        template <typename COORD>
        inline void __coord_add(COORD &C, const coord1d value) throw()
        {
            coord1d *q = __coord_addr(C);
            for(size_t dim=0;dim<YOCTO_FAME_DIM_OF(COORD);++dim)
            {
                q[dim] += value;
            }
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
        
        inline coord1d next_rank1d(const coord1d rank, const coord1d rmax) throw()
        {
            if(rank>=rmax) return 0; else return rank+1;
        }
        
        inline coord1d prev_rank1d(const coord1d rank, const coord1d rmax) throw()
        {
            if(rank<=0) return rmax; else return rank-1;
        }

        //______________________________________________________________________
        //
        //
        //
        //______________________________________________________________________
        
        template <typename T,typename COORD>
        struct vertex_for;
        
        template <typename T>
        struct vertex_for<T,coord1d>
        {
            typedef T type;
        };
        
        template <typename T>
        struct vertex_for<T,coord2d>
        {
            typedef point2d<T> type;
        };
        
        template <typename T>
        struct vertex_for<T,coord3d>
        {
            typedef point3d<T> type;
        };
        
        //______________________________________________________________________
        //
        //
        //
        //______________________________________________________________________
        
        template <typename T,typename COORD>
        class box
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            YOCTO_FAME_DECL_COORD;
            
            typedef typename vertex_for<T,COORD>::type        vtx;
            typedef typename type_traits<vtx>::parameter_type param_vtx;
            typedef          const vtx                        const_vtx;
            
            
            const_vtx  lower;
            const_vtx  upper;
            const_vtx  width;
            const_type volume;
            
            inline virtual ~box() throw() {}
            inline explicit box( param_vtx lo, param_vtx up ) throw() :
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
            
            inline box( const box &other ) throw() :
            lower(other.lower),
            upper(other.upper),
            volume(other.volume)
            {
            }
            
            
            inline const_type *__lower() const throw() { return (const_type *) &lower; }
            inline const_type *__upper() const throw() { return (const_type *) &upper; }
            inline const_type *__width() const throw() { return (const_type *) &width; }
            
            
            
            inline friend std::ostream & operator<<( std::ostream &os, const box &p )
            {
                os << "| " << p.lower << " -> " << p.upper << ", volume=" << p.volume << " |";
                return os;
            }
            
            
        private:
            YOCTO_DISABLE_ASSIGN(box);
        };

        
        
    }
    
}

#endif
