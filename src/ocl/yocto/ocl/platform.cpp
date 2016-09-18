#include "yocto/ocl/platform.hpp"
#include "yocto/string/tokenizer.hpp"

#include <iostream>

namespace yocto
{
    namespace ocl
    {
        static const char oclGetPlatformInfoText[] = "clGetPlatformInfo";

        Platform:: ~Platform() throw()
        {
        }


        static inline bool __is_ext_sep(char C) throw() { return ' ' == C; }

        void __ocl_parse_extensions( _Extensions &Ext, const string &extensions)
        {
            assert(0==Ext.size);
            size_t num_extensions = 0;
            {
                tokenizer tkn(extensions);
                while( tkn.get_next(__is_ext_sep))
                {
                    ++num_extensions;
                }
            }
            
            Ext.resize_empty_to(num_extensions);
            {
                tokenizer tkn(extensions);
                while(tkn.get_next(__is_ext_sep))
                {
                    const string ext = tkn.to_string();
                    Ext.push_back(ext);
                }
            }
        }
        
#define  Y_OCL_INI(NAME) NAME(GetInfo<cl_platform_id,cl_platform_info>::String(ID,CL_PLATFORM_##NAME,clGetPlatformInfo,oclGetPlatformInfoText))

        Platform:: Platform(const cl_platform_id platform_id) :
        ID(platform_id),
        Y_OCL_INI(PROFILE),
        Y_OCL_INI(VERSION),
        Y_OCL_INI(NAME),
        Y_OCL_INI(VENDOR),
        EXTENSIONS(),
        Devices()
        {
            //__________________________________________________________________
            //
            // get EXTENSIONS
            //__________________________________________________________________

            const string extensions = GetInfo<cl_platform_id,cl_platform_info>::String(ID,CL_PLATFORM_EXTENSIONS,clGetPlatformInfo,oclGetPlatformInfoText);
            _Extensions &Ext = (_Extensions&)EXTENSIONS;
            __ocl_parse_extensions(Ext,extensions);
            
            //__________________________________________________________________
            //
            // query Devices
            //__________________________________________________________________
            cl_uint num_devices = 0;
            {
                const cl_int err = clGetDeviceIDs(ID,CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices);
                YOCTO_OCL_CHECK(err,"clGetDeviceIDs - Level 1");
            }

            memory::buffer_of<cl_device_id,memory::pooled> devices_id(num_devices);
            {
                const cl_int err = clGetDeviceIDs(ID,CL_DEVICE_TYPE_ALL, num_devices,devices_id(), NULL);
                YOCTO_OCL_CHECK(err,"clGetDeviceIDs - Level 2");
            }

            //__________________________________________________________________
            //
            // create Devices
            //__________________________________________________________________
            _Devices &Dev = (_Devices &)Devices;
            Dev.resize_empty_to(num_devices);
            for(cl_uint i=0;i<num_devices;++i)
            {
                Dev.append<cl_device_id>(devices_id[i]);
            }

        }

        
    }
}
