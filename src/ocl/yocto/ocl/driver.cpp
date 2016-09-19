#include "yocto/ocl/driver.hpp"

namespace yocto
{
    namespace ocl
    {
        Driver:: ~Driver() throw()
        {
        }

        const Device & Driver:: QueryDevice( const cl_device_id id ) const
        {
            for(cl_uint i=0;i<Platforms.size;++i)
            {
                for(cl_uint j=0;i<Platforms[i].Devices.size;++j)
                {
                    const Device &D = Platforms[i].Devices[j];
                    if(id==D.ID)
                    {
                        return D;
                    }
                }
            }
            throw exception("%s QueryDevice(unknown cl_device_id)", name);
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


        void CL_CALLBACK Driver::Notify(cl_program program, void *user_data)
        {
            //assert(user_data);
            //Driver &self = *(Driver *)user_data;

        }

        static const char oclGetProgramBuildInfo[] = "clGetProgramBuildInfo";

        template <typename T>
        static inline
        T __ReadBuildValue( cl_program program, cl_device_id device, cl_program_build_info param )
        {
            T ans(0);
            const size_t param_value_size = sizeof(T);
            const cl_int err              = clGetProgramBuildInfo(program, device, param, param_value_size, &ans, NULL);
            YOCTO_OCL_CHECK(err,oclGetProgramBuildInfo);
            return ans;
        }

        static inline
        size_t __ReadBuildLength(cl_program program, cl_device_id device, cl_program_build_info param)
        {
            size_t param_value_ret_size = 0;
            const cl_int err = clGetProgramBuildInfo(program, device, param, 0, NULL, &param_value_ret_size);
            YOCTO_OCL_CHECK(err,oclGetProgramBuildInfo);
            return param_value_ret_size;
        }

        static inline
        string __ReadBuildString(cl_program program, cl_device_id device, cl_program_build_info param)
        {
            const size_t len = __ReadBuildLength(program,device,param);
            memory::buffer_of<char,memory::pooled> buf(len);
            const cl_int err = clGetProgramBuildInfo(program, device, param, len, buf(), NULL);
            YOCTO_OCL_CHECK(err,oclGetProgramBuildInfo);
            return string(buf(),len);
        }



        void Driver:: BuildProgram(Program      &program,
                                   cl_uint       num_devices,
                                   cl_device_id *device_list,
                                   const string &options)
        {
            BuildLogs.resize(num_devices);


            const cl_int err = clBuildProgram(*program, num_devices, device_list, &options[0], NULL, NULL);
            for(cl_uint i=0;i<num_devices;++i)
            {
                // get device
                const cl_device_id device = device_list[i];
                const Device      &D      = QueryDevice(device);
                string            &Log    = BuildLogs.push_back(D.NAME);
                Log += "\n";

                // get status
                const cl_build_status status = __ReadBuildValue<cl_build_status>(*program,device,CL_PROGRAM_BUILD_STATUS);
                switch(status)
                {
                    case CL_BUILD_SUCCESS: Log += "\tSUCCESS\n"; break;
                    case CL_BUILD_ERROR:   Log += "\tFAILURE\n"; break;
                    default:               Log += "\tUnknown...\n"; break;
                }

                Log += __ReadBuildString(*program, device, CL_PROGRAM_BUILD_LOG);
                Log += "\n";
            }
            YOCTO_OCL_CHECK(err, "clBuildProgram" );
        }


        void Driver:: BuildProgram(Program         &program,
                                   const DeviceMap &device_map,
                                   const string    &options)
        {
            const size_t num_devices = device_map.size;
            memory::buffer_of<cl_device_id,memory::pooled> devices(num_devices);
            size_t idev = 0;
            for( const addr_node<const Device> *node = device_map.head;node;node=node->next)
            {
                devices[idev++] = node->addr->ID;
            }
            BuildProgram(program,num_devices,devices(),options);
        }

        void Driver:: BuildProgram(Program &program, const Device &device, const string &options)
        {
            BuildProgram(program, 1, (cl_device_id *)&device.ID, options);
        }

    }
}
