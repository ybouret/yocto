#ifndef YOCTO_IPSO_COORD_INCLUDED
#define YOCTO_IPSO_COORD_INCLUDED 1

/**
 Interface Parallel Splitting Object
 */

#include "yocto/math/point3d.hpp"
#include "yocto/type/args.hpp"

namespace yocto
{
    namespace ipso
    {

        template <typename T>
        struct coord
        {
            YOCTO_ARGUMENTS_DECL_T;
            
            typedef type          _1;
            typedef point2d<type> _2;
            typedef point3d<type> _3;

            //! access
            template <typename COORD>
            static inline T & get(COORD &C, const size_t dim) throw()
            {
                assert(dim<sizeof(COORD)/sizeof(T)); return ((type*)&C)[dim];
            }

            //! const access
            template <typename COORD>
            static inline const_type & get(const COORD &C, const size_t dim) throw()
            {
                assert(dim<sizeof(COORD)/sizeof(T));
                return ((const_type *)&C)[dim];
            }

            //! decrease all coordinates
            template <typename COORD>
            static inline COORD dec(COORD C) throw()
            {
                type *q = (type *)&C;
                for(size_t i=0;i<sizeof(COORD)/sizeof(T);++i)
                {
                    --q[i];
                }
                return C;
            }

            //! for integral types
            template <typename COORD>
            static inline bool are_same(const COORD &lhs, const COORD &rhs) throw()
            {
                const_type *L = (const_type *)&lhs;
                const_type *R = (const_type *)&rhs;
                for(size_t i=0;i<sizeof(COORD)/sizeof(T);++i)
                {
                    if(L[i]!=R[i]) return false;
                }
                return true;
            }

        };
    }
}

#endif

