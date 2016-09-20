#ifndef YOCTO_OCL_BUFFER_INCLUDED
#define YOCTO_OCL_BUFFER_INCLUDED 1

#include "yocto/ocl/context.hpp"
#include "yocto/ocl/mem-object.hpp"

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

        private:
            YOCTO_DISABLE_ASSIGN(BufferOf);
        };

    }
}
#endif
