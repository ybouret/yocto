#ifndef YOCTO_OCL_KERNEL_INCLUDED
#define YOCTO_OCL_KERNEL_INCLUDED 1

#include "yocto/ocl/program.hpp"
#include "yocto/math/point3d.hpp"

namespace yocto
{
    namespace ocl
    {
        typedef Shared<cl_kernel> _Kernel;

        class Kernel : public _Kernel
        {
        public:
            typedef GetInfo<cl_kernel,cl_kernel_info> GetInfoType;

            virtual ~Kernel() throw();
            explicit Kernel(const Program &program, const string &kernel_name);
            explicit Kernel(const Program &program, const char   *kernel_name);

            const string  FUNCTION_NAME;
            const cl_uint NUM_ARGS;
            
        private:
            YOCTO_DISABLE_ASSIGN(Kernel);
            static cl_kernel Create(const Program &program, const string &kernel_name);
            static cl_kernel Create(const Program &program, const char   *kernel_name);

        public:
            class WorkGroupInfo
            {
            public:
                explicit WorkGroupInfo(const Kernel &kernel, const Device &device);
                virtual ~WorkGroupInfo() throw();

                const size_t          WORK_GROUP_SIZE;
                const point3d<size_t> COMPILE_WORK_GROUP_SIZE;
                const cl_ulong        LOCAL_MEM_SIZE;
                const size_t          PREFERRED_WORK_GROUP_SIZE_MULTIPLE;
                const cl_ulong        PRIVATE_MEM_SIZE;

                WorkGroupInfo(const WorkGroupInfo &other) throw();

            private:
                YOCTO_DISABLE_ASSIGN(WorkGroupInfo);
            };
        };
    }
}


#endif
