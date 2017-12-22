#ifndef YOCTO_ISQRT_INCLUDED
#define YOCTO_ISQRT_INCLUDED 1

#include "yocto/os.hpp"

namespace yocto
{
    template <typename T> inline
    T iSquareRoot(const T n) throw()
    {
        if(n<=1)
        {
            return n;
        }
        else
        {
            T x0 = n;
            T x1 = (n>>1);
            while(true)
            {
                x0 = x1;
                x1 = ((x0+n/x0)>>1);
                if(x1>=x0)
                {
                    return x0;
                }
            }
        }

    }
}

#endif

