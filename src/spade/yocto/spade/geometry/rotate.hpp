#ifndef YOCTO_SPADE_GEO_ROTATE_INCLUDED
#define YOCTO_SPADE_GEO_ROTATE_INCLUDED 1

#include "yocto/spade/types

namespace yocto
{
    namespace spade
    {

        struct geo
        {
            //! rotate in 2D
            template <typename VTX>
            static inline
            VTX rotate( const VTX &v, const typename VTX::type theta ) throw()
            {
                const typename VTX::type ct = math::Cos(theta);
                const typename VTX::type st = math::Sin(theta);
                VTX p(v);
                p.x = ct * v.x - st*v.y;
                p.y = st * v.x + ct*v.y;
                return p;
            }
        };
    }
}

#endif
