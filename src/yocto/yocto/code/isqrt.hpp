#ifndef YOCTO_ISQRT_INCLUDED
#define YOCTO_ISQRT_INCLUDED 1

#include "yocto/code/utils.hpp"

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
            T x1 = n/2+1;
            do
            {
                x0 = x1;
                x1 = (x0+n/x0)/2;
            }
            //while(!core::are_separated_by_less_than_one(x0,x1));
            while( 1 < ((x0<x1) ? (x1-x0) : (x0-x1)) );
            return min_of(x0,x1);
        }

    }
}

#endif

