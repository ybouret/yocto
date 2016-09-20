#ifndef YOCTO_OCL_KERNEL_INCLUDED
#define YOCTO_OCL_KERNEL_INCLUDED 1

#include "yocto/ocl/program.hpp"

namespace yocto
{
    namespace ocl
    {
        typedef Shared<cl_kernel> _Kernel;

        class Kernel : public _Kernel
        {
        public:
            virtual ~Kernel() throw();
            explicit Kernel(const Program &program, const string &kernel_name);
            explicit Kernel(const Program &program, const char   *kernel_name);

        private:
            YOCTO_DISABLE_ASSIGN(Kernel);
            static cl_kernel Create(const Program &program, const string &kernel_name);
            static cl_kernel Create(const Program &program, const char   *kernel_name);
        };
    }
}


#endif
