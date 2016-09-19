#ifndef YOCTO_OCL_BUFFER_INCLUDED
#define YOCTO_OCL_BUFFER_INCLUDED 1

#include "yocto/ocl/context.hpp"
#include "yocto/ocl/mem-object.hpp"
#include "yocto/ocl/command-queue.hpp"

namespace yocto
{
    namespace ocl
    {
        class Buffer : public MemObject
        {
        public:
            explicit Buffer(const Context     &context,
                            const cl_mem_flags flags,
                            const size_t       size,
                            const void        *host_ptr);

            virtual ~Buffer() throw();


            void EnqueueRead(CommandQueue  Q,
                             const cl_bool blocking_read,
                             void         *target,
                             const size_t  bytes,
                             const size_t  offset) const;

            void EnqueueWrite(CommandQueue  Q,
                              const cl_bool blocking_write,
                              const void   *source,
                              const size_t  bytes,
                              const size_t  offset) const;

        private:
            YOCTO_DISABLE_ASSIGN(Buffer);
            static cl_mem Create(cl_context         context,
                                 const cl_mem_flags flags,
                                 const size_t       size,
                                 const void        *host_ptr);

        };

        template <typename T>
        class BufferOf : public Buffer
        {
        public:
            const size_t ITEMS;
            inline virtual ~BufferOf() throw() {}

            inline explicit BufferOf(const Context     &context,
                                     const cl_mem_flags flags,
                                     const size_t       num_items,
                                     const T           *items) :
            Buffer(context,flags,num_items*sizeof(T),items),
            ITEMS(SIZE/sizeof(T))
            {
            }

            inline explicit BufferOf(const BufferOf &other) :
            Buffer(other),
            ITEMS(other.ITEMS)
            {
            }

            inline void EnqueueReadItems(CommandQueue &Q,
                                         const cl_bool blocking_read,
                                         T            *target,
                                         const size_t  count,
                                         const size_t  start) const
            {
                EnqueueRead(Q,blocking_read,target+start, count*sizeof(T), 0);
            }

            inline void EnqueueWriteItems(CommandQueue &Q,
                                          const cl_bool blocking_write,
                                          const T     * source,
                                          const size_t  count,
                                          const size_t  start) const
            {
                EnqueueWrite(Q,blocking_write,source+start, count*sizeof(T), 0);
            }


        private:
            YOCTO_DISABLE_ASSIGN(BufferOf);
        };

    }
}
#endif
