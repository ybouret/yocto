#ifndef YOCTO_RANDOMIZED_UNIFORM1_INCLUDED
#define YOCTO_RANDOMIZED_UNIFORM1_INCLUDED 1

#include "yocto/randomized/bits.hpp"

namespace yocto
{
    namespace Randomized
    {
        class Uniform1 : public Bits
        {
        public:
            explicit Uniform1() throw();
            virtual ~Uniform1() throw();

            virtual uint32_t next32() throw();

        private:
            int32_t  seed_;
            int32_t  iy_;
            int32_t  iv_[32];
            YOCTO_DISABLE_COPY_AND_ASSIGN(Uniform1);
            void initialize() throw();
        };
    }
}

#endif

