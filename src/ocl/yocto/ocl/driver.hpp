#ifndef YOCTO_OCL_DRIVER_INCLUDED
#define YOCTO_OCL_DRIVER_INCLUDED 1

#include "yocto/threading/singleton.hpp"
#include "yocto/ocl/command-queue.hpp"
#include "yocto/sequence/slots.hpp"

namespace yocto
{
    namespace ocl
    {
        class Driver : public singleton<Driver>
        {
        public:
            typedef slots_of<Platform> _Platforms;
            const _Platforms Platforms;

        private:
            virtual ~Driver() throw();
            explicit Driver();
            YOCTO_DISABLE_COPY_AND_ASSIGN(Driver);
            friend class singleton<Driver>;
            static const char name[];
            static const threading::longevity life_time = 7;
            static const cl_uint              max_num_platforms = 8;
        };
    }
}

#endif
