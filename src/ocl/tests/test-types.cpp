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

    std::cerr << std::endl;
    std::cerr << "Creating Context..." << std::endl;
    const ocl::Platform &platform = OpenCL.Platforms[0];
    const ocl::_Devices &Devices  = platform.Devices;
    if(Devices.size<=0)
    {
        throw exception("No OpenCL devices...");
    }

    for(size_t i=0;i<Devices.size;++i)
    {
        std::cerr << "Using " << Devices[i].NAME << std::endl;
        ocl::Context context(platform,Devices[i]);
    }

    ocl::DeviceMap       devmap;
    // try cpu
    for(size_t i=0;i<Devices.size;++i)
    {
        const ocl::Device &D = platform.Devices[i];
        if(D.is_cpu())
        {
            std::cerr << "Using CPU " << D.NAME << std::endl;
            devmap.append(&D);
            break;
        }
    }

    if(devmap.size<=0)
    {
        const ocl::Device &D = Devices[0];
        std::cerr << "Using device " << D.NAME << std::endl;
        devmap.append(&D);
    }

    {
        std::cerr << "Creating Context" << std::endl;
        ocl::Context      context(platform,devmap);

        std::cerr << "Creating Command Queue" << std::endl;
        ocl::CommandQueue Q(context,*(devmap.head->addr),0);
    }



    

}
YOCTO_UNIT_TEST_DONE()
