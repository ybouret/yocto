#ifndef YOCTO_RANDOMIZED_UNIFORM_MT_INCLUDED
#define YOCTO_RANDOMIZED_UNIFORM_MT_INCLUDED 1

#include "yocto/randomized/bits.hpp"

namespace yocto
{
    namespace Randomized
    {
        class UniformMT : public Bits
        {
        public:
            explicit UniformMT() throw();
            virtual ~UniformMT() throw();

            virtual uint32_t next32() throw();
            virtual void     reseed(Bits &bits) throw();

        private:
            int           mti;
            unsigned long mt[624];
            YOCTO_DISABLE_COPY_AND_ASSIGN(UniformMT);
            void initialize(const unsigned long) throw();
        };
    }
}

#endif

