#ifndef YOCTO_IOS_NULL_DEVICE_INCLUDED
#define YOCTO_IOS_NULL_DEVICE_INCLUDED 1

#include "yocto/os.hpp"
#include <cstdio>

namespace yocto
{
    namespace ios
    {
#define YOCTO_NULL_DEVICE "/dev/null"
#if defined(YOCTO_WIN)
#undef YOCTO_NULL_DEVICE
#define YOCTO_NULL_DEVICE "NUL:"
#endif
        struct null_device
        {
            static void for_cstdin();
            static void for_cstderr();
            static void for_cstdout();
        };
    }

}


#endif

