#ifndef YOCTO_OPENCL_COMMAND_QUEUE_INCLUDED
#define YOCTO_OPENCL_COMMAND_QUEUE_INCLUDED 1

#include "yocto/ocl/kernel.hpp"

namespace yocto
{
    namespace ocl
    {

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

            void EnqueueReadBuffer(Buffer       &buffer,
                                   const cl_bool blocking,
                                   void         *target,
                                   const size_t  bytes,
                                   const size_t  offset);

            void EnqueueWriteBuffer(Buffer       &buffer,
                                    const cl_bool blocking,
                                    const void   *source,
                                    const size_t  bytes,
                                    const size_t  offset);

            template <typename T>
            inline void EnqueueReadItems(BufferOf<T>  &buffer,
                                         const cl_bool blocking,
                                         T            *target,
                                         const size_t  count,
                                         const size_t  start)
            {
                EnqueueReadBuffer(buffer,blocking,target,count*sizeof(T),start*sizeof(T));
            }

            template <typename T>
            inline void EnqueueWriteItems(BufferOf<T>  &buffer,
                                          const cl_bool blocking,
                                          const T      *source,
                                          const size_t  count,
                                          const size_t  start)
            {
                EnqueueWriteBuffer(buffer,blocking,source,count*sizeof(T),start*sizeof(T));
            }




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
