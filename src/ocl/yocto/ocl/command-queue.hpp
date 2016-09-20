#ifndef YOCTO_OPENCL_COMMAND_QUEUE_INCLUDED
#define YOCTO_OPENCL_COMMAND_QUEUE_INCLUDED 1

#include "yocto/ocl/context.hpp"

namespace yocto
{
    namespace ocl
    {
        class Kernel;

        typedef Shared<cl_command_queue> _CommandQueue;

        class CommandQueue : public _CommandQueue
        {
        public:
            virtual ~CommandQueue() throw();
            explicit CommandQueue(const Context &context,
                                  const Device  &device,
                                  cl_command_queue_properties properties);
            CommandQueue(const CommandQueue &Q);

            void Flush();
            void Finish();

            void EnqueueNDRangeKernel(const Kernel  &kernel,
                                      const cl_uint  work_dim,
                                      const size_t  *global_work_offset,
                                      const size_t  *global_work_size,
                                      const size_t  *local_work_size);

            void EnqueueTask(const Kernel &kernel);


        private:
            YOCTO_DISABLE_ASSIGN(CommandQueue);
            static cl_command_queue Create(const Context &context,
                                           const Device  &device,
                                           cl_command_queue_properties properties);
            
        };
    }
}

#endif
