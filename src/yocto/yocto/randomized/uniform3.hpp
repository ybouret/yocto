#ifndef YOCTO_RANDOMIZED_UNIFORM3_INCLUDED
#define YOCTO_RANDOMIZED_UNIFORM3_INCLUDED 1

#include "yocto/randomized/bits.hpp"

namespace yocto
{
    namespace Randomized
    {
        class Uniform3 : public Bits
        {
        public:
            explicit Uniform3() throw();
            virtual ~Uniform3() throw();

            virtual uint32_t next32() throw();

        private:
            int32_t  inext_;
            int32_t  inextp_;
            int32_t  ma_[56];
            YOCTO_DISABLE_COPY_AND_ASSIGN(Uniform3);
            void initialize() throw();
        };
    }
}

#endif

