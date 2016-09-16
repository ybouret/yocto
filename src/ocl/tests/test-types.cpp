#include "yocto/ocl/types.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;

YOCTO_UNIT_TEST_IMPL(types)
{
    cl_platform_id platforms[8];
    cl_uint        max_num_platforms = sizeof(platforms)/sizeof(platforms[0]);
    cl_uint        num_platforms = 0;
    cl_int err = clGetPlatformIDs(max_num_platforms, platforms, &num_platforms);

    if( err != CL_SUCCESS )
    {
        throw ocl::Exception( err, "clGetPlaftormIDs" );
    }
    std::cerr << "#platform=" << num_platforms << std::endl;

}
YOCTO_UNIT_TEST_DONE()
