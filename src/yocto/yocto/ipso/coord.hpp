#ifndef YOCTO_IPSO_COORD_INCLUDED
#define YOCTO_IPSO_COORD_INCLUDED 1

/**
 Interface for Parallel Splitting of Objects
 */

#include "yocto/math/point3d.hpp"
#include "yocto/type/args.hpp"

namespace yocto
{
    namespace ipso
    {

        typedef unit_t           coord1D;
        typedef point2d<coord1D> coord2D;
        typedef point3d<coord1D> coord3D;

        template <typename COORD>
        inline coord1D & __coord(COORD &C,const size_t dim) throw()
        {
            assert(dim<sizeof(COORD)/sizeof(coord1D)); return ((coord1D*)&C)[dim];
        }

        template <typename COORD>
        inline const coord1D & __coord(const COORD &C,const size_t dim) throw()
        {
            assert(dim<sizeof(COORD)/sizeof(coord1D)); return ((coord1D*)&C)[dim];
        }

        template <typename COORD>
        inline bool __coord_eq(const COORD &lhs, const COORD &rhs) throw()
        {
            const coord1D  *L = (const coord1D  *)&lhs;
            const coord1D  *R = (const coord1D  *)&rhs;
            for(size_t i=0;i<sizeof(COORD)/sizeof(coord1D);++i)
            {
                if(L[i]!=R[i]) return false;
            }
            return true;
        }

        template <typename COORD>
        inline void __coord_dec(COORD &C) throw()
        {
            coord1D  *q = (coord1D *)&C;
            for(size_t i=0;i<sizeof(COORD)/sizeof(coord1D);++i)
            {
                --q[i];
            }
        }

        template <typename COORD>
        inline COORD __coord_abs(COORD C) throw()
        {
            coord1D  *q = (coord1D *)&C;
            for(size_t i=0;i<sizeof(COORD)/sizeof(coord1D);++i)
            {
                const coord1D value = q[i];
                if(value<0) q[i] = -value;
            }
            return C;
        }

        template <typename COORD>
        coord1D __coord_prod(COORD C) throw()
        {
            coord1D  *q   = (coord1D *)&C;
            coord1D   p = q[0];
            for(size_t i=1;i<sizeof(COORD)/sizeof(coord1D);++i)
            {
                p *= q[i];
            }
            return p;
        }

        template <typename COORD>
        coord1D __coord_sum(COORD C) throw()
        {
            coord1D  *q   = (coord1D *)&C;
            coord1D   p = q[0];
            for(size_t i=1;i<sizeof(COORD)/sizeof(coord1D);++i)
            {
                p += q[i];
            }
            return p;
        }

        inline const char * __coord_name(const size_t dim) throw()
        {
            switch(dim)
            {
                case 0: return "x";
                case 1: return "y";
                case 2: return "z";
                default:
                    break;
            }
            return "?";
        }

    }
}

#endif

