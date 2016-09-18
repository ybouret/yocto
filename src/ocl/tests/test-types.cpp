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
        std::cerr << "\tNAME       = " << P.NAME       << std::endl;
        std::cerr << "\tVERSION    = " << P.VERSION    << std::endl;
        std::cerr << "\tVENDOR     = " << P.VENDOR     << std::endl;
        std::cerr << "\tPROFILE    = " << P.PROFILE    << std::endl;
        std::cerr << "\tEXTENSIONS = " << P.EXTENSIONS << std::endl;
        std::cerr << "#Devices     = " << P.Devices.size << std::endl;
        for(size_t j=0;j<P.Devices.size;++j)
        {
            const ocl::Device &D = P.Devices[j];
            std::cerr << "\t\tDevice[" << j << "]" << std::endl;
            std::cerr << "\t\t\tNAME       = " << D.NAME           << std::endl;
            std::cerr << "\t\t\tVENDOR     = " << D.VENDOR         << std::endl;
            std::cerr << "\t\t\tVERSION    = " << D.VERSION        << std::endl;
            std::cerr << "\t\t\tDRIVER     = " << D.DRIVER_VERSION << std::endl;
            std::cerr << "\t\t\tEXTENSIONS = " << D.EXTENSIONS     << std::endl;
        }
     }

}
YOCTO_UNIT_TEST_DONE()
