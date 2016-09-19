#ifndef YOCTO_OCL_DEVICE_INCLUDED
#define YOCTO_OCL_DEVICE_INCLUDED 1

#include "yocto/ocl/types.hpp"
#include "yocto/sequence/slots.hpp"
#include "yocto/sequence/addr-list.hpp"

namespace yocto
{
    namespace ocl
    {
        typedef slots_of<const string> _Extensions;
        void __ocl_parse_extensions( _Extensions &Ext, const string &extensions);

        class Device
        {
        public:
            typedef slots_of<const size_t> MaxWorkItemSizes;
            Device(const cl_device_id device_id);
            ~Device() throw();
            const cl_device_id                ID;
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
            const _Extensions                 EXTENSIONS;
            const cl_bool                     COMPILER_AVAILABLE;
            const cl_bool                     LINKER_AVAILABLE;


            inline bool is_cpu() const throw() { return 0 !=(TYPE & CL_DEVICE_TYPE_CPU); }
            inline bool is_gpu() const throw() { return 0 !=(TYPE & CL_DEVICE_TYPE_GPU); }
            bool        is_available() const;
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Device);
        };
        
        typedef addr_list<const Device> DeviceMap;
        
    }
}

#endif
