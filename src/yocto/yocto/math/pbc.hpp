#ifndef YOCTO_MATH_PBC_INCLUDED
#define YOCTO_MATH_PBC_INCLUDED 1

#include "yocto/math/point3d.hpp"
#include <math.h>

namespace yocto
{
    struct PBC
    {
        static inline double On(const double x, const double L) throw() { return x - L * floor(0.5 + x/L); }
        static inline float  On(const float  x, const float  L) throw() { return x - L * floorf(0.5f+x/L); }

        static inline double On(const double x, const double L, const double invL) throw() { return x - L * floor(0.5+x*invL);   }
        static inline float  On(const float  x, const float  L, const float  invL) throw() { return x - L * floorf(0.5f+x*invL); }

        template <typename VERTEX>
        static inline void OnX(VERTEX &v,const VERTEX &Box) throw() { v.x = On(v.x,Box.x); }

        template <typename VERTEX>
        static inline void OnXY(VERTEX &v,const VERTEX &Box) throw() { v.x = On(v.x,Box.x); v.y=On(v.y,Box.y); }

        template <typename VERTEX>
        static inline void OnXYZ(VERTEX &v,const VERTEX &Box) throw() { v.x = On(v.x,Box.x); v.y=On(v.y,Box.y); v.z=On(v.z,Box.z); }
    };

}

#endif


