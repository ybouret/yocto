#include "yocto/ocl/mem-object.hpp"

namespace yocto
{
    namespace ocl
    {
        template <> void _MemObject::Retain(cl_mem m) throw()
        {
            (void) clRetainMemObject(m);
        }


        template <> void _MemObject:: Release(cl_mem m) throw()
        {
            (void) clReleaseMemObject(m);
        }


        MemObject:: ~MemObject() throw()
        {
        }

        static  const char oclGetMemObjectInfoText[] = "clGetMemObjectInfo";
#define Y_OCL_INI(TYPE,FIELD) FIELD( GetInfoType::Read<TYPE>::Value(handle,CL_MEM_##FIELD,clGetMemObjectInfo,oclGetMemObjectInfoText) )

#define Y_OCL_MEM_OBJECT()          \
Y_OCL_INI(cl_mem_object_type,TYPE), \
Y_OCL_INI(size_t,SIZE)

        MemObject:: MemObject( cl_mem m ) : _MemObject(m),
        Y_OCL_MEM_OBJECT()
        {
        }



        MemObject:: MemObject(const MemObject &other) : _MemObject(other),
        Y_OCL_MEM_OBJECT()
        {
        }
        


    }
}
