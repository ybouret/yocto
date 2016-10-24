#include "yocto/threading/scheme/simd.hpp"

namespace yocto
{
    namespace threading
    {
        SIMD:: ~SIMD() throw()
        {
        }

        SIMD:: SIMD( executor &xxx ) throw() :
        cpu(xxx)
        {
        }
        
    }
}

namespace yocto
{
    namespace threading
    {
        SIMD1:: ~SIMD1() throw()
        {
        }

        SIMD1:: SIMD1() throw() :
        seq_executor(),
        SIMD(static_cast<executor&>(*this))
        {
        }

        void SIMD1:: run( kernel &k )
        {
            k(ctx);
        }
    }
}
