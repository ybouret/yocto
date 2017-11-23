#ifndef YOCTO_RANDOMIZED_GENERATOR_INCLUDED
#define YOCTO_RANDOMIZED_GENERATOR_INCLUDED 1

#include "yocto/randomized/bits.hpp"

namespace yocto
{
    namespace Randomized
    {
        template <typename T, typename BITS>
        class Generator : public BITS
        {
        public:
            inline explicit Generator() throw() : BITS()
            {
            }

            inline virtual ~Generator() throw() {}

            inline T operator()(void) throw()
            {
                return this->template to<T>();
            }
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Generator);
        };
    }
}

#endif

