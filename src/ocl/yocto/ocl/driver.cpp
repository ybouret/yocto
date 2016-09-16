#include "yocto/ocl/driver.hpp"

namespace yocto
{
    namespace ocl
    {
        Driver:: ~Driver() throw()
        {
        }

        Driver:: Driver() : Platforms()
        {
            // query #platforms
            cl_platform_id platforms[max_num_platforms];
            cl_uint        num_platforms = 0;

            const cl_int   err = clGetPlatformIDs(max_num_platforms, platforms, &num_platforms);
            if(CL_SUCCESS!=err)
            {
                throw ocl::Exception(err,"clGetPlatformIDs");
            }

            // create the platforms
            _Platforms &P = (_Platforms &)Platforms;
            P.resize_empty_to(num_platforms);

            for(cl_uint i=0;i<num_platforms;++i)
            {
                const cl_platform_id platform_id = platforms[i];
                P.append<cl_platform_id>(platform_id);
            }
            
        }

        const char Driver::name[] = "OpenCL";
    }
}
