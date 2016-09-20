#include "yocto/ocl/buffer.hpp"

namespace yocto
{
    namespace ocl
    {

        Buffer:: ~Buffer() throw()
        {
        }


        Buffer:: Buffer(const Context     &context,
                        const cl_mem_flags flags,
                        const size_t       size,
                        const void        *host_ptr) :
        MemObject( Create( *context, flags, size, host_ptr) )
        {
        }

        cl_mem Buffer:: Create(cl_context context, const cl_mem_flags flags, const size_t size, const void *host_ptr)
        {
            cl_int err = CL_SUCCESS;
            cl_mem ans = clCreateBuffer(context, flags, size, (void*)host_ptr, &err);
            YOCTO_OCL_CHECK(err, "clCreateBuffer");
            return ans;
        }

        
    }

}
