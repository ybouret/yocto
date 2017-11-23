#ifndef YOCTO_RANDOMIZED_UNIFORM2_INCLUDED
#define YOCTO_RANDOMIZED_UNIFORM2_INCLUDED 1

#include "yocto/randomized/bits.hpp"

namespace yocto
{
    namespace Randomized
    {
        class Uniform2 : public Bits
        {
        public:
            explicit Uniform2() throw();
            virtual ~Uniform2() throw();

            virtual uint32_t next32() throw();

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Uniform2);

            int32_t  seed_;
            int32_t  saux_;
            int32_t  iy_;
            int32_t  iv_[32];

            void initialize() throw();
        };
    }
}

#endif

