#ifndef YOCTO_OCL_TYPES_INCLUDED
#define YOCTO_OCL_TYPES_INCLUDED 1


#include "yocto/string.hpp"
#include "yocto/exception.hpp"

#if defined(YOCTO_APPLE)
#       include <OpenCL/cl.h>
#else
#       include <CL/cl.h>
#endif

namespace yocto
{
    namespace ocl
    {
        struct Error
        {
            static const char *String(const cl_int err) throw();
        };

        class Exception : public exception
        {
        public:
            const cl_int code;
            explicit Exception(const cl_int err,const char *fmt,...) throw();
            virtual ~Exception() throw();
            Exception(const Exception &other) throw();
            virtual const char *what() const throw();

        private:
            YOCTO_DISABLE_ASSIGN(Exception);
        };
    }
}

#endif
