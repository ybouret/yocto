#ifndef YOCTO_OCL_PLATFORM_INCLUDED
#define YOCTO_OCL_PLATFORM_INCLUDED 1

#include "yocto/ocl/types.hpp"

namespace yocto
{
    namespace ocl
    {
        class Platform
        {
        public:
            ~Platform() throw();
            Platform(const cl_platform_id platform_id);

            const cl_platform_id id;
            const string         PROFILE;
            const string         VERSION;
            const string         NAME;
            const string         VENDOR;
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Platform);
            static const char clGetPlatformInfoText[];
        };
    }
}

#endif
