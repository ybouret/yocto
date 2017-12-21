#ifndef YOCTO_ISQRT_INCLUDED
#define YOCTO_ISQRT_INCLUDED 1

#include "yocto/os.hpp"

namespace yocto
{
    template <typename T>
    T iSquareRoot(const T n) throw()
    {
        if(n<=1)
        {
            return n;
        }
        else
        {
            T x0 = n;
            T x1 = n/2+1;
            return x1;
        }

    }
}

#endif

