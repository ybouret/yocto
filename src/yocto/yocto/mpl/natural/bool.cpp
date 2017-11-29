#include "yocto/mpl/natural.hpp"
#include "yocto/code/unroll.hpp"

namespace yocto
{
    namespace mpl
    {

        natural natural::booleanOps(const void *lhs, const size_t nl,
                                    const void *rhs, const size_t nr,
                                    uint8_t (*proc)(const uint8_t, const uint8_t))
        {
            const uint8_t *pLarge = static_cast<const uint8_t *>(lhs);
            const uint8_t *pSmall = static_cast<const uint8_t *>(rhs);
            size_t         nLarge = nl;
            size_t         nSmall = nr;
            if(nLarge<nSmall)
            {
                cswap(pLarge,pSmall);
                cswap(nLarge,nSmall);
            }

            natural ans(nLarge,as_capacity);
            for(size_t i=nSmall;i<nLarge;++i)
            {
                ans.byte[i] = proc(pLarge[i],0);
            }
            for(size_t i=0;i<nSmall;++i)
            {
                ans.byte[i] = proc(pLarge[i],pSmall[i]);
            }
            ans.rescan();

            return ans;
        }

        static inline uint8_t proc_OR(const uint8_t a, const uint8_t b) throw()
        {
            return (a|b);
        }

        natural natural:: OR(const void *lhs, const size_t nl,
                             const void *rhs, const size_t nr)
        {
            return booleanOps(lhs,nl,rhs,nr,proc_OR);
        }

        static inline uint8_t proc_AND(const uint8_t a, const uint8_t b) throw()
        {
            return (a&b);
        }

        natural natural:: AND(const void *lhs, const size_t nl,
                              const void *rhs, const size_t nr)
        {
            return booleanOps(lhs,nl,rhs,nr,proc_AND);
        }

    }
}

