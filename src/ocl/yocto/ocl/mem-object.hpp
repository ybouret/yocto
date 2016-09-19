#ifndef YOCTO_MEM_OBJECT_INCLUDED
#define YOCTO_MEM_OBJECT_INCLUDED 1


#include "yocto/ocl/types.hpp"

namespace yocto
{
    namespace ocl
    {
        typedef Shared<cl_mem> _MemObject;
        class MemObject : public _MemObject
        {
        public:
            typedef GetInfo<cl_mem,cl_mem_info> GetInfoType;

            virtual ~MemObject() throw();
            MemObject(const MemObject &);

            const cl_mem_object_type  TYPE;
            const size_t              SIZE;
            
        protected:
            explicit MemObject(cl_mem m);
            
        private:
            YOCTO_DISABLE_ASSIGN(MemObject);
        };
    }
}

#endif
