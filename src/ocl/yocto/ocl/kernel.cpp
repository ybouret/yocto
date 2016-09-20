#include "yocto/ocl/kernel.hpp"
#include "yocto/code/bzset.hpp"

namespace yocto
{
    namespace ocl
    {
        template <>
        void _Kernel::Retain(cl_kernel k) throw()
        {
            (void) clRetainKernel(k);
        }

        template <>
        void _Kernel::Release(cl_kernel k) throw()
        {
            (void) clReleaseKernel(k);
        }

        Kernel:: ~Kernel() throw()
        {}

        cl_kernel Kernel::Create(const Program &program, const string &kernel_name)
        {
            cl_int    err = CL_SUCCESS;
            cl_kernel k   = clCreateKernel(*program, &kernel_name[0], &err);
            YOCTO_OCL_CHECK(err, "clCreateKernel" );
            return k;
        }

        cl_kernel Kernel::Create(const Program &program, const char *kernel_name)
        {
            const string kid(kernel_name);
            return Create(program,kid);
        }

        static const char oclGetKernelInfoText[] = "clGetKernelInfo";

#define Y_OCL_STR(FIELD) FIELD( GetInfoType::String(handle,CL_KERNEL_##FIELD,clGetKernelInfo,oclGetKernelInfoText) )
#define Y_OCL_INI(TYPE,FIELD) FIELD( GetInfoType::Read<TYPE>::Value(handle,CL_KERNEL_##FIELD,clGetKernelInfo,oclGetKernelInfoText) )

        

#define Y_OCL_KERNEL_CTOR()             \
_Kernel( Create(program,kernel_name) ), \
Y_OCL_STR(FUNCTION_NAME),               \
Y_OCL_INI(cl_uint,NUM_ARGS)


        Kernel:: Kernel(const Program &program, const string &kernel_name) :
        Y_OCL_KERNEL_CTOR()
        {
        }

        Kernel:: Kernel(const Program &program, const char   *kernel_name) :
        Y_OCL_KERNEL_CTOR()
        {
        }

        template <typename T>
        static inline T __ocl_kernel_work_group_info(const Kernel                   &kernel,
                                                     const Device                   &device,
                                                     const cl_kernel_work_group_info param)
        {
            T ans; bzset(ans);
            const cl_int err = clGetKernelWorkGroupInfo(*kernel, device.ID, param, sizeof(T), &ans, NULL);
            YOCTO_OCL_CHECK(err, "clGetKernelWorkGroupInfo" );
            return ans;
        }

#undef Y_OCL_INI
#define Y_OCL_INI(TYPE,FIELD) FIELD( __ocl_kernel_work_group_info<TYPE>(kernel,device,CL_KERNEL_##FIELD) )

        Kernel:: WorkGroupInfo:: WorkGroupInfo(const Kernel &kernel, const Device &device) :
        Y_OCL_INI(size_t,WORK_GROUP_SIZE),
        Y_OCL_INI(point3d<size_t>,COMPILE_WORK_GROUP_SIZE),
        Y_OCL_INI(cl_ulong,LOCAL_MEM_SIZE),
        Y_OCL_INI(size_t,PREFERRED_WORK_GROUP_SIZE_MULTIPLE),
        Y_OCL_INI(cl_ulong,PRIVATE_MEM_SIZE)
        {
        }

        Kernel:: WorkGroupInfo:: ~WorkGroupInfo() throw()
        {}


        void Kernel:: __SetArg(const cl_uint arg_index,
                               const size_t  arg_size,
                               const void   *arg_value)
        {
            const cl_int err = clSetKernelArg(handle,arg_index,arg_size,arg_value);
            YOCTO_OCL_CHECK(err, "clSetKernelArg" );
        }

        void Kernel:: SetBuffer(const cl_uint arg_index, const Buffer &buf)
        {
            cl_mem m = *buf;
            __SetArg(arg_index, sizeof(cl_mem), &m);
        }


    }
}
