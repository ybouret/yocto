#ifndef YOCTO_OCL_DRIVER_INCLUDED
#define YOCTO_OCL_DRIVER_INCLUDED 1

#include "yocto/threading/singleton.hpp"
#include "yocto/ocl/buffer.hpp"
#include "yocto/ocl/program.hpp"
#include "yocto/sequence/slots.hpp"

namespace yocto
{
    namespace ocl
    {
        class Driver : public singleton<Driver>
        {
        public:
            typedef slots_of<Platform>    _Platforms;
            typedef dynamic_slots<string> _BuildLogs;
            const _Platforms Platforms;
            _BuildLogs       BuildLogs;

            const Device & QueryDevice( const cl_device_id id ) const;

            void BuildProgram(Program         &program,
                              const DeviceMap &devmap,
                              const string    &options);
            
            void BuildProgram(Program         &program,
                              const Device    &device,
                              const string    &options);

        private:
            virtual ~Driver() throw();
            explicit Driver();
            YOCTO_DISABLE_COPY_AND_ASSIGN(Driver);
            friend class singleton<Driver>;
            static const char name[];
            static const threading::longevity life_time = 7;
            static const cl_uint              max_num_platforms = 8;
            
            

            void BuildProgram(Program      &program,
                              cl_uint       num_devices,
                              cl_device_id *device_list,
                              const string &options);
            


        };
    }
}

#endif
