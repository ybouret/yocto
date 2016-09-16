#ifndef YOCTO_OCL_DEVICE_INCLUDED
#define YOCTO_OCL_DEVICE_INCLUDED 1

#include "yocto/ocl/types.hpp"
#include "yocto/sequence/slots.hpp"

namespace yocto
{
    namespace ocl
    {
        typedef slots_of<const string> _Extensions;

        class Device
        {
        public:
            typedef slots_of<const size_t> MaxWorkItemSizes;
            Device(const cl_device_id device_id);
            ~Device() throw();
            const cl_device_id                id;
            const cl_device_type              TYPE;
            const cl_uint                     VENDOR_ID;
            const cl_uint                     MAX_COMPUTE_UNITS;
            const cl_uint                     MAX_WORK_ITEM_DIMENSIONS;
            const MaxWorkItemSizes            MAX_WORK_ITEM_SIZES;
            const size_t                      MAX_WORK_GROUP_SIZE;
            const cl_uint                     MAX_CLOCK_FREQUENCY;
            const cl_ulong                    MAX_MEM_ALLOC_SIZE;
            const size_t                      MAX_PARAMETER_SIZE;
            const cl_ulong                    GLOBAL_MEM_SIZE;
            const cl_ulong                    LOCAL_MEM_SIZE;
            const cl_device_exec_capabilities EXECUTION_CAPABILITIES;
            const cl_command_queue_properties QUEUE_PROPERTIES;
            const cl_platform_id              PLATFORM;
            const string                      NAME;
            const string                      VENDOR;
            const string                      DRIVER_VERSION;
            const string                      VERSION;
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Device);
        };
    }
}

#endif
