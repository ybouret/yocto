#include "yocto/ocl/context.hpp"

namespace yocto
{
    namespace ocl
    {
        Context:: ~Context() throw()
        {
#if 0
            const cl_int err = clReleaseContext(context);
            if(err!=CL_SUCCESS)
            {
                std::cerr << "Release Context: " << Error::String(err) << std::endl;
            }
#endif
            (void) clReleaseContext(context);
            context = NULL;
        }
        
        Context:: Context(const Platform &P, const DeviceMap &device_map) :
        context(0)
        {
            const size_t num_devices = device_map.size;
            memory::buffer_of<cl_device_id,memory::pooled> devices(num_devices);
            size_t idev = 0;
            for( const addr_node<const Device> *node = device_map.head;node;node=node->next)
            {
                devices[idev++] = node->addr->ID;
            }
            setup(P,devices(),num_devices);
        }
        
        Context:: Context(const Platform &P, const Device &D)
        {
            setup(P, &D.ID, 1);
        }
        
        CL_CALLBACK void ContextCB(const char *errinfo,
                                   const void *private_info,
                                   size_t      cb,
                                   void       *user_data)
        {
            std::cerr << "errInfo=" << errinfo << std::endl;
        }
        
        
        void Context:: setup(const Platform      &,
                             const cl_device_id *devices,
                             const size_t        num_devices)
        {
            cl_int err = CL_SUCCESS;
            context    = clCreateContext(NULL, num_devices, devices, ContextCB, NULL, &err);
            YOCTO_OCL_CHECK(err,"clCreateContext");
        }
        
    }
}
