#include "yocto/ocl/types.hpp"

namespace yocto
{
    namespace ocl
    {
        
#define Y_OCL_ERR(X) case X: return #X

        const char * Error::String(const cl_int err) throw()
        {
            switch(err)
            {
                    Y_OCL_ERR(CL_SUCCESS);
                    Y_OCL_ERR(CL_DEVICE_NOT_FOUND);
                    Y_OCL_ERR(CL_DEVICE_NOT_AVAILABLE);
                    Y_OCL_ERR(CL_COMPILER_NOT_AVAILABLE);
                    Y_OCL_ERR(CL_MEM_OBJECT_ALLOCATION_FAILURE);
                    Y_OCL_ERR(CL_OUT_OF_RESOURCES);
                    Y_OCL_ERR(CL_OUT_OF_HOST_MEMORY);
                    Y_OCL_ERR(CL_PROFILING_INFO_NOT_AVAILABLE);
                    Y_OCL_ERR(CL_MEM_COPY_OVERLAP);
                    Y_OCL_ERR(CL_IMAGE_FORMAT_MISMATCH);
                    Y_OCL_ERR(CL_IMAGE_FORMAT_NOT_SUPPORTED);
                    Y_OCL_ERR(CL_BUILD_PROGRAM_FAILURE);
                    Y_OCL_ERR(CL_MAP_FAILURE);
                    Y_OCL_ERR(CL_MISALIGNED_SUB_BUFFER_OFFSET);
                    Y_OCL_ERR(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST);
#if defined(CL_VERSION_1_2)
                    Y_OCL_ERR(CL_COMPILE_PROGRAM_FAILURE);
                    Y_OCL_ERR(CL_LINKER_NOT_AVAILABLE);
                    Y_OCL_ERR(CL_LINK_PROGRAM_FAILURE);
                    Y_OCL_ERR(CL_DEVICE_PARTITION_FAILED);
                    Y_OCL_ERR(CL_KERNEL_ARG_INFO_NOT_AVAILABLE);
#endif
                    Y_OCL_ERR(CL_INVALID_VALUE);
                    Y_OCL_ERR(CL_INVALID_DEVICE_TYPE);
                    Y_OCL_ERR(CL_INVALID_PLATFORM);
                    Y_OCL_ERR(CL_INVALID_DEVICE);
                    Y_OCL_ERR(CL_INVALID_CONTEXT);
                    Y_OCL_ERR(CL_INVALID_QUEUE_PROPERTIES);
                    Y_OCL_ERR(CL_INVALID_COMMAND_QUEUE);
                    Y_OCL_ERR(CL_INVALID_HOST_PTR);
                    Y_OCL_ERR(CL_INVALID_MEM_OBJECT);
                    Y_OCL_ERR(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
                    Y_OCL_ERR(CL_INVALID_IMAGE_SIZE);
                    Y_OCL_ERR(CL_INVALID_SAMPLER);
                    Y_OCL_ERR(CL_INVALID_BINARY);
                    Y_OCL_ERR(CL_INVALID_BUILD_OPTIONS);
                    Y_OCL_ERR(CL_INVALID_PROGRAM);
                    Y_OCL_ERR(CL_INVALID_PROGRAM_EXECUTABLE);
                    Y_OCL_ERR(CL_INVALID_KERNEL_NAME);
                    Y_OCL_ERR(CL_INVALID_KERNEL_DEFINITION);
                    Y_OCL_ERR(CL_INVALID_KERNEL);
                    Y_OCL_ERR(CL_INVALID_ARG_INDEX);
                    Y_OCL_ERR(CL_INVALID_ARG_VALUE);
                    Y_OCL_ERR(CL_INVALID_ARG_SIZE);
                    Y_OCL_ERR(CL_INVALID_KERNEL_ARGS);
                    Y_OCL_ERR(CL_INVALID_WORK_DIMENSION);
                    Y_OCL_ERR(CL_INVALID_WORK_GROUP_SIZE);
                    Y_OCL_ERR(CL_INVALID_WORK_ITEM_SIZE);
                    Y_OCL_ERR(CL_INVALID_GLOBAL_OFFSET);
                    Y_OCL_ERR(CL_INVALID_EVENT_WAIT_LIST);
                    Y_OCL_ERR(CL_INVALID_EVENT);
                    Y_OCL_ERR(CL_INVALID_OPERATION);
                    Y_OCL_ERR(CL_INVALID_GL_OBJECT);
                    Y_OCL_ERR(CL_INVALID_BUFFER_SIZE);
                    Y_OCL_ERR(CL_INVALID_MIP_LEVEL);
                    Y_OCL_ERR(CL_INVALID_GLOBAL_WORK_SIZE);
                    Y_OCL_ERR(CL_INVALID_PROPERTY);
#if defined(CL_VERSION_1_2)
                    Y_OCL_ERR(CL_INVALID_IMAGE_DESCRIPTOR);
                    Y_OCL_ERR(CL_INVALID_COMPILER_OPTIONS);
                    Y_OCL_ERR(CL_INVALID_LINKER_OPTIONS);
                    Y_OCL_ERR(CL_INVALID_DEVICE_PARTITION_COUNT);
#endif
                default:
                    break;
            }
            return "Unknown Error";
        }
        
        
    }
}

namespace yocto
{
    namespace ocl
    {

        Exception:: ~Exception() throw()
        {}

        Exception:: Exception(const Exception &other) throw() : exception(other), code(other.code)
        {
        }

        Exception:: Exception(const cl_int err,const char *fmt,...) throw() : exception(), code(err)
        {
            va_list ap;
            va_start(ap,fmt);
            format(fmt,&ap);
        }

        const char *Exception:: what() const throw()
        {
            return Error::String(code);
        }


    }

}



