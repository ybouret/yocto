#ifndef YOCTO_OCL_CONTEXT_INCLUDED
#define YOCTO_OCL_CONTEXT_INCLUDED 1

#include "yocto/ocl/platform.hpp"
#include "yocto/ocl/device.hpp"

namespace yocto
{
    namespace ocl
    {
        typedef Shared<cl_context> _Context;

        class Context : public _Context
        {
        public:
            Context(const Platform &P, const DeviceMap &device_map);
            ~Context() throw();
            
            Context(const Platform &P, const Device &D);
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Context);
            
            static cl_context Create(const Platform &P, const DeviceMap &device_map);
            static cl_context Setup(const Platform &P, const cl_device_id *devices, const size_t num_devices);


        };
    }
}

#endif
