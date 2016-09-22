#include "yocto/threading/vpu.hpp"

namespace yocto
{
    namespace threading
    {

        vpu:: ~vpu() throw()
        {
        }


        vpu:: vpu(kernel_executor *kxp) throw() :
        simd(kxp),
        cores(simd->num_threads())
        {
        }

        kernel_executor & vpu:: operator*() throw()
        {
            return *simd;
        }

        const kernel_executor & vpu:: operator*() const throw()
        {
            return *simd;
        }

    }
}
