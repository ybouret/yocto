#include "yocto/ocl/driver.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/sequence/vector.hpp"

using namespace yocto;

static const char ocl_add_prolog[] =
" __kernel void add( __global float *a) {\n";

static const char ocl_add_code[] =
"	const size_t i = get_global_id(0);\n"
"	a[i] += i;\n"
;

static const char ocl_add_epilog[] =
"}\n";



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
            std::cerr << std::endl;
            std::cerr << "Using CPU " << D.NAME << std::endl;
            devmap.append(&D);
            break;
        }
    }

    if(devmap.size<=0)
    {
        const ocl::Device &D = Devices[0];
        std::cerr << std::endl;
        std::cerr << "Using device " << D.NAME << std::endl;
        devmap.append(&D);
    }

    {
        std::cerr << "Creating Context" << std::endl;
        ocl::Context      context(platform,devmap);

        std::cerr << "Creating Command Queue" << std::endl;
        ocl::CommandQueue Q(context,*(devmap.head->addr),0);

        std::cerr << "Creating Buffer" << std::endl;

        ocl::BufferOf<float> buf(context,CL_MEM_READ_WRITE,100,NULL);

        std::cerr << "Creating Data" << std::endl;
        vector<float> data(buf.ITEMS);

        std::cerr << "Enqueue Write..." << std::endl;
        //buf.EnqueueWrite(Q, CL_FALSE, &data[1], buf.SIZE, 0);
        std::cerr << "Enqueue Write Items..." << std::endl;
        //buf.EnqueueWriteItems(Q,CL_FALSE, &data[1], 100, 0);

        std::cerr << "Enqueue Read..." << std::endl;
        //buf.EnqueueRead(Q, CL_FALSE, &data[1], buf.SIZE, 0);
        std::cerr << "Enqueue Read Items..." << std::endl;
       // buf.EnqueueReadItems(Q,CL_FALSE, &data[1], 100, 0);

        std::cerr << "Creating sources" << std::endl;
        ocl::Sources sources;
        sources.push_back(ocl_add_prolog);
        sources.push_back(ocl_add_code);
        sources.push_back(ocl_add_epilog);
        std::cerr << "sources=" << sources << std::endl;
        std::cerr << "Creating program..." << std::endl;
        ocl::Program program(context,sources);

        std::cerr << "Build Program..." << std::endl;
        const string options;
        try
        {
            OpenCL.BuildProgram(program, devmap, options);
            std::cerr << "SUCCESS BuildLogs:" << std::endl;
            std::cerr << OpenCL.BuildLogs << std::endl;
        }
        catch(...)
        {
            std::cerr << "FAILURE BuildLogs:" << std::endl;
            std::cerr << OpenCL.BuildLogs << std::endl;
            throw;
        }
    }



    

}
YOCTO_UNIT_TEST_DONE()
