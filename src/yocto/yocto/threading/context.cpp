#include "yocto/threading/context.hpp"


namespace yocto
{
    namespace threading
    {
        context:: ~context() throw() {}

        context:: context(const size_t the_rank,
                          const size_t the_size,
                          lockable    &the_lock) throw() :
        rank(the_rank),
        size(the_size),
        access(the_lock),
        indx(rank+1),
        priv(0)
        {
            assert(size>0);
            assert(rank<size);
        }

        
    }
    
}

namespace yocto
{
    namespace threading
    {
        context_supply:: ~context_supply() throw()
        {
        }

        context_supply:: context_supply(const size_t n, lockable &l ) :
        slots_of<context>(n)
        {
            assert(n>0);
            for(size_t r=0;r<n;++r)
            {
                append<size_t,size_t,lockable&>(r,n,l);
            }
        }

    }
}
