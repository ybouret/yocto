#include "yocto/ocl/context.hpp"

namespace yocto
{
    namespace ocl
    {
        template <>
        void _Context::Release(cl_context context) throw()
        {
            (void)clReleaseContext(context);
        }

        Context:: ~Context() throw()
        {
        }

        Context:: Context(const Platform &P, const DeviceMap &device_map) :
        _Context( Create(P,device_map) )
        {

        }

        Context:: Context(const Platform &P, const Device &D) :
        _Context( Setup(P, &D.ID, 1) )
        {
        }

        CL_CALLBACK void ContextCB(const char *errinfo,
                                   const void *private_info,
                                   size_t      cb,
                                   void       *user_data)
        {
            std::cerr << "errInfo=" << errinfo << std::endl;
        }


        cl_context Context:: Create(const Platform &P, const DeviceMap &device_map)
        {
            const size_t num_devices = device_map.size;
            memory::buffer_of<cl_device_id,memory::pooled> devices(num_devices);
            size_t idev = 0;
            for( const addr_node<const Device> *node = device_map.head;node;node=node->next)
            {
                devices[idev++] = node->addr->ID;
            }
            return Setup(P,devices(),num_devices);
        }

        cl_context Context:: Setup(const Platform      &,
                                   const cl_device_id *devices,
                                   const size_t        num_devices)
        {
            cl_int      err = CL_SUCCESS;
            cl_context  ctx = clCreateContext(NULL, num_devices, devices, ContextCB, NULL, &err);
            YOCTO_OCL_CHECK(err,"clCreateContext");
            return ctx;
        }
        
        
        
    }
}
