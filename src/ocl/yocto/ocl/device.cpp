#include "yocto/ocl/device.hpp"
#include <iostream>

namespace yocto
{
    namespace ocl
    {
        Device:: ~Device() throw()
        {
        }

        static const char oclGetDeviceInfoText[] = "clGetDeviceInfo";

#define Y_OCL_INI(TYPE,FIELD) FIELD(GetInfo<cl_device_id,cl_device_info>::Read<TYPE>::Value(ID,CL_DEVICE_##FIELD,clGetDeviceInfo,oclGetDeviceInfoText))
#define Y_OCL_STR(FIELD) FIELD(GetInfo<cl_device_id,cl_device_info>::String(ID,CL_DEVICE_##FIELD,clGetDeviceInfo,oclGetDeviceInfoText))

        Device:: Device( const cl_device_id device_id ) :
        ID(device_id),
        Y_OCL_INI(cl_device_type,TYPE),
        Y_OCL_INI(cl_uint,VENDOR_ID),
        Y_OCL_INI(cl_uint,MAX_COMPUTE_UNITS),
        Y_OCL_INI(cl_uint,MAX_WORK_ITEM_DIMENSIONS),
        MAX_WORK_ITEM_SIZES(MAX_WORK_ITEM_DIMENSIONS),
        Y_OCL_INI(size_t,MAX_WORK_GROUP_SIZE),
        Y_OCL_INI(cl_uint,MAX_CLOCK_FREQUENCY),
        Y_OCL_INI(cl_ulong,MAX_MEM_ALLOC_SIZE),
        Y_OCL_INI(size_t,MAX_PARAMETER_SIZE),
        Y_OCL_INI(cl_ulong,GLOBAL_MEM_SIZE),
        Y_OCL_INI(cl_ulong,LOCAL_MEM_SIZE),
        Y_OCL_INI(cl_device_exec_capabilities,EXECUTION_CAPABILITIES),
        Y_OCL_INI(cl_command_queue_properties,QUEUE_PROPERTIES),
        Y_OCL_INI(cl_platform_id,PLATFORM),
        Y_OCL_STR(NAME),
        Y_OCL_STR(VENDOR),
        DRIVER_VERSION(GetInfo<cl_device_id,cl_device_info>::String(ID,CL_DRIVER_VERSION,clGetDeviceInfo,oclGetDeviceInfoText)),
        Y_OCL_STR(VERSION),
        EXTENSIONS()
        {
            for(cl_uint i=0;i<MAX_WORK_ITEM_DIMENSIONS;++i)
            {
                ((MaxWorkItemSizes &)MAX_WORK_ITEM_SIZES).push_back(0);
            }
            const size_t bytes =  MAX_WORK_ITEM_DIMENSIONS*sizeof(size_t);
            const void  *caddr = &MAX_WORK_ITEM_SIZES[0];
            cl_int       err   = clGetDeviceInfo(ID, CL_DEVICE_MAX_WORK_ITEM_SIZES, bytes,(void*)caddr, NULL);
            YOCTO_OCL_CHECK(err,oclGetDeviceInfoText);
            
            const string extensions = GetInfo<cl_device_id,cl_device_info>::String(ID,CL_DEVICE_EXTENSIONS,clGetDeviceInfo, oclGetDeviceInfoText);
            __ocl_parse_extensions((_Extensions &)EXTENSIONS,extensions);
        }
        
    }
}
