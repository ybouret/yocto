#ifndef YOCTO_RANDOMIZED_URAND32_INCLUDED
#define YOCTO_RANDOMIZED_URAND32_INCLUDED 1

#include "yocto/randomized/bits.hpp"

namespace yocto
{
    namespace Randomized
    {

        //! Marsaglia's 32 bit random generators
        class __rand32
        {
        public:
            uint32_t z,w,jsr,jcong,a,b;
            uint32_t x,y,bro;
            uint8_t  c;
            uint32_t t[256];

            explicit __rand32() throw();
            virtual ~__rand32() throw();

            typedef uint32_t (__rand32::*generator)();
            uint32_t mwc()   throw();
            uint32_t shr3()  throw();
            uint32_t cong()  throw();
            uint32_t fib()   throw();
            uint32_t kiss()  throw();
            uint32_t lfib4() throw();
            uint32_t swb()   throw();

            void settable( uint32_t i1, uint32_t i2, uint32_t i3, uint32_t i4, uint32_t i5, uint32_t i6 ) throw();
            void reset( uint32_t s ) throw();


            static void test();

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(__rand32);
        };

    }
}

#endif

