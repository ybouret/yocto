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

        void Buffer::EnqueueRead(CommandQueue  Q,
                                 const cl_bool blocking_read,
                                 void         *target,
                                 const size_t  bytes,
                                 const size_t  offset) const
        {
            const cl_int err = clEnqueueReadBuffer(*Q,handle, blocking_read, offset, bytes, target, 0, NULL, NULL);
            YOCTO_OCL_CHECK(err, "clEnqueueReadBuffer");
        }

        void Buffer:: EnqueueWrite(CommandQueue  Q,
                                   const cl_bool blocking_write,
                                   const void   *source,
                                   const size_t  bytes,
                                   const size_t  offset) const
        {
            const cl_int err = clEnqueueWriteBuffer(*Q,handle, blocking_write, offset, bytes, source, 0, NULL, NULL);
            YOCTO_OCL_CHECK(err, "clEnqueueWriteBuffer");

        }

    }

}
