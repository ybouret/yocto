#ifndef YOCTO_ISQRT_INCLUDED
#define YOCTO_ISQRT_INCLUDED 1

#include "yocto/code/utils.hpp"

namespace yocto
{
    namespace core
    {
        template <typename T> inline
        bool are_separated_by_less_than_one(const T lhs, const T rhs) throw()
        {
            if(lhs<rhs)
            {
                return (rhs-lhs)<=1;
            }
            else
            {
                if(rhs<lhs)
                {
                    return (lhs-rhs)<=1;
                }
                else
                {
                    return true; // lhs==rhs
                }
            }
        }

    }

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
                //std::cerr << "x0=" << x0 << ", x1=" << x1 << std::endl;
            } while(!core::are_separated_by_less_than_one(x0,x1));

            return min_of(x0,x1);
        }

    }
}

#endif

