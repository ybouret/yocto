#include "yocto/ocl/kernel.hpp"

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

        Kernel:: Kernel(const Program &program, const string &kernel_name) :
        _Kernel( Create(program,kernel_name) )
        {
        }

        Kernel:: Kernel(const Program &program, const char   *kernel_name) :
        _Kernel( Create(program,kernel_name) )
        {
        }


    }
}
