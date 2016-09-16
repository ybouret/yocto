#include "yocto/ocl/platform.hpp"
#include <iostream>

namespace yocto
{
    namespace ocl
    {
        const char Platform:: clGetPlatformInfoText[] = "clGetPlatformInfo";

        Platform:: ~Platform() throw()
        {
        }


#define  Y_OCL_INI(NAME) NAME(GetInfo<cl_platform_id,cl_platform_info>::String(id,CL_PLATFORM_##NAME,clGetPlatformInfo,clGetPlatformInfoText))

        Platform:: Platform(const cl_platform_id platform_id) :
        id(platform_id),
        Y_OCL_INI(PROFILE),
        Y_OCL_INI(VERSION),
        Y_OCL_INI(NAME),
        Y_OCL_INI(VENDOR)
        {
        }

        
    }
}
