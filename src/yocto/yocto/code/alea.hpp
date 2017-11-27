#ifndef YOCTO_CODE_ALEA_INCLUDED
#define YOCTO_CODE_ALEA_INCLUDED 1

#include "yocto/randomized/urand32.hpp"

namespace yocto
{
    struct alea
    {
        static Randomized::Kiss32 bits;
    };
}

#endif

