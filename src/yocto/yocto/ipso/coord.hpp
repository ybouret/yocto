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

            template <typename COORD>
            static inline T & get(COORD &C, const size_t dim) throw()
            {
                assert(dim<sizeof(COORD)/sizeof(T)); return ((type*)&C)[dim];
            }

            template <typename COORD>
            static inline const_type & get(const COORD &C, const size_t dim) throw()
            {
                assert(dim<sizeof(COORD)/sizeof(T));
                return ((const_type *)&C)[dim];
            }
        };
    }
}

#endif

