#include "yocto/ocl/driver.hpp"

namespace yocto
{
    namespace ocl
    {
        Driver:: ~Driver() throw()
        {
        }

        Driver:: Driver()
        {
            cl_platform_id platforms[max_num_platforms];
            cl_uint        num_platforms = 0;

            const cl_int   err = clGetPlatformIDs(max_num_platforms, platforms, &num_platforms);
            if(CL_SUCCESS!=err)
            {
                throw ocl::Exception(err,"clGetPlatformIDs");
            }
        }

        const char Driver::name[] = "OpenCL";
    }
}
