#ifndef YOCTO_RANDOMIZED_UNIFORM0_INCLUDED
#define YOCTO_RANDOMIZED_UNIFORM0_INCLUDED 1

#include "yocto/randomized/bits.hpp"

namespace yocto
{
    namespace Randomized
    {
        class Uniform0 : public Bits
        {
        public:
            explicit Uniform0() throw();
            virtual ~Uniform0() throw();

            virtual uint32_t next32() throw();
            virtual void     reseed(Bits &bits) throw();

        private:
            int32_t   seed_;
            YOCTO_DISABLE_COPY_AND_ASSIGN(Uniform0);
            
        };
    }
}

#endif

