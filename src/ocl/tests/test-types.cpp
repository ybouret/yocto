#include "yocto/ocl/driver.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;

YOCTO_UNIT_TEST_IMPL(types)
{
    ocl::Driver &OpenCL = ocl::Driver::instance();
    const size_t num_platforms = OpenCL.Platforms.size;
    std::cerr << "num_platforms = " << num_platforms << std::endl;
    for(size_t i=0;i<num_platforms;++i)
    {
        const ocl::Platform &P = OpenCL.Platforms[i];
        std::cerr << "\tNAME    = " << P.NAME    << std::endl;
        std::cerr << "\tVERSION = " << P.VERSION << std::endl;
        std::cerr << "\tVENDOR  = " << P.VENDOR  << std::endl;
        std::cerr << "\tPROFILE = " << P.PROFILE << std::endl;
     }

}
YOCTO_UNIT_TEST_DONE()
