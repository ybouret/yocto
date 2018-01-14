#ifndef YOCTO_IOS_NULL_DEVICE_INCLUDED
#define YOCTO_IOS_NULL_DEVICE_INCLUDED 1

#include "yocto/os.hpp"
#include <cstdio>

#define YOCTO_NULL_DEVICE "/dev/null"
#if defined(YOCTO_WIN)
#undef YOCTO_NULL_DEVICE
#define YOCTO_NULL_DEVICE "NUL:"
#endif


#endif

