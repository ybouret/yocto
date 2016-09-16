#ifndef YOCTO_OCL_PLATFORM_INCLUDED
#define YOCTO_OCL_PLATFORM_INCLUDED 1

#include "yocto/ocl/device.hpp"

namespace yocto
{
    namespace ocl
    {
        class Platform
        {
        public:
            typedef slots_of<const Device> _Devices;

            ~Platform() throw();
            Platform(const cl_platform_id platform_id);

            const cl_platform_id       id;            //!< this id
            const string               PROFILE;       //!< CL_PLATFORM_PROFILE
            const string               VERSION;       //!< CL_PLATFORM_VERSION
            const string               NAME;          //!< CL_PLATFORM_NAME
            const string               VENDOR;        //!< CL_PLATFORM_VENDOR
            const _Extensions          EXTENSIONS;    //!< CL_PLATFORM_EXTENSIONS, PARSED!
            const _Devices             Devices;


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Platform);
        };
    }
}

#endif
