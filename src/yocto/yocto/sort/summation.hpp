#ifndef YOCTO_SUMMATION_INCLUDED
#define YOCTO_SUMMATION_INCLUDED 1

#include "yocto/sort/quick.hpp"

namespace yocto
{
    template <
    typename T
    >
    T summation( T *a, size_t n) throw()
    {
        assert(!(NULL==a)&&n>0);
        switch(n)
        {
            case 0: return 0;
            case 1: return a[0];
            default:
                break;
        }
        quicksort(a,n,__compare_decreasing<T>);
        T sum(0);
        while(n-->0)
        {
            sum += a[n];
        }
        return sum;
    }

    template <
    typename T
    >
    T summation( array<T> &a, const size_t n) throw()
    {
        assert(n<=a.size());
        switch(n)
        {
            case  0: return 0;
            case  1: return a[1];
            default: break;
        }
        lw_array<T> arr( &a[1], n );
        quicksort(arr,__compare_decreasing<T>);
        T sum(0);
#define YOCTO_SUMMATION(I) sum += arr[I]
        YOCTO_LOOP_FUNC_(n,YOCTO_SUMMATION,1);
#undef  YOCTO_SUMMATION
        return sum;
    }

    
}

#endif


