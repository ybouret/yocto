#include "yocto/ocl/platform.hpp"
#include "yocto/string/tokenizer.hpp"

#include <iostream>

namespace yocto
{
    namespace ocl
    {
        const char Platform:: clGetPlatformInfoText[] = "clGetPlatformInfo";

        Platform:: ~Platform() throw()
        {
        }


        static inline bool __is_ext_sep(char C) throw() { return ' ' == C; }

#define  Y_OCL_INI(NAME) NAME(GetInfo<cl_platform_id,cl_platform_info>::String(id,CL_PLATFORM_##NAME,clGetPlatformInfo,clGetPlatformInfoText))

        Platform:: Platform(const cl_platform_id platform_id) :
        id(platform_id),
        Y_OCL_INI(PROFILE),
        Y_OCL_INI(VERSION),
        Y_OCL_INI(NAME),
        Y_OCL_INI(VENDOR),
        EXTENSIONS()
        {
            const string extensions = GetInfo<cl_platform_id,cl_platform_info>::String(id,CL_PLATFORM_EXTENSIONS,clGetPlatformInfo,clGetPlatformInfoText);
            _Extensions &Ext = (_Extensions&)EXTENSIONS;
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

        
    }
}
