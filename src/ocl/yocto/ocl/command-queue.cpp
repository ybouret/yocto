#include "yocto/ocl/command-queue.hpp"

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
            const cl_int err = clFinish(handle);
            YOCTO_OCL_CHECK(err, "clFinish" );
        }
    }
}
