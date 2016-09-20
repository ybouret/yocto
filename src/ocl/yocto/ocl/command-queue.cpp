#include "yocto/ocl/command-queue.hpp"
#include "yocto/ocl/kernel.hpp"

namespace yocto
{
    namespace ocl
    {
        template <>
        void _CommandQueue:: Release(cl_command_queue Q) throw()
        {
            (void) clReleaseCommandQueue(Q);
        }

        template <>
        void _CommandQueue:: Retain(cl_command_queue Q) throw()
        {
            (void) clRetainCommandQueue(Q);
        }

        CommandQueue:: ~CommandQueue() throw()
        {
        }

        CommandQueue:: CommandQueue(const Context &context,
                                    const Device  &device,
                                    cl_command_queue_properties properties) :
        _CommandQueue( Create(context,device,properties) )
        {
        }

        CommandQueue::CommandQueue(const CommandQueue &Q) : _CommandQueue(Q)
        {
        }

        cl_command_queue CommandQueue:: Create(const Context              &context,
                                               const Device               &device,
                                               cl_command_queue_properties properties)
        {
            cl_int           err = CL_SUCCESS;
            cl_command_queue Q   = clCreateCommandQueue(*context, device.ID, properties, &err);
            YOCTO_OCL_CHECK(err,"clCreateCommandQueue");
            return Q;
        }


        void CommandQueue:: Finish()
        {
            YOCTO_OCL_RUN(clFinish,(handle));
        }

        void CommandQueue:: Flush()
        {
            YOCTO_OCL_RUN(clFlush,(handle));
        }


        void CommandQueue:: EnqueueNDRangeKernel(const Kernel  &kernel,
                                                 const cl_uint  work_dim,
                                                 const size_t  *global_work_offset,
                                                 const size_t  *global_work_size,
                                                 const size_t  *local_work_size)
        {
            YOCTO_OCL_RUN(clEnqueueNDRangeKernel,(handle, *kernel, work_dim, global_work_offset, global_work_size, local_work_size,YOCTO_OCL_NO_EVENTS));
        }

        void CommandQueue:: EnqueueTask(const Kernel &kernel)
        {
            YOCTO_OCL_RUN(clEnqueueTask,(handle, *kernel,YOCTO_OCL_NO_EVENTS));
        }


    }
}
