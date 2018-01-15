#include "yocto/ios/null-device.hpp"
#include "yocto/exceptions.hpp"

namespace yocto
{
    namespace ios
    {
        static inline void __check(FILE *stream,const char *id)
        {
            const int err = ferror(stream);
            if(err!=0)
            {
                throw libc::exception(err,"%s=freopen(%s)",id,YOCTO_NULL_DEVICE);
            }
        }

        void null_device:: for_stdin()
        {
            YOCTO_LOCK(access);

#if defined(YOCTO_WIN)
            (void) freopen(YOCTO_NULL_DEVICE,"r",stdin);
#else
            stdin = freopen(YOCTO_NULL_DEVICE,"r",stdin);
#endif
            __check(stdin,"stdin");
        }

        void null_device:: for_stderr()
        {
            YOCTO_LOCK(access);

#if defined(YOCTO_WIN)
            (void) freopen(YOCTO_NULL_DEVICE,"w",stderr);
#else
            stderr = freopen(YOCTO_NULL_DEVICE,"w",stderr);
#endif
            __check(stderr,"stderr");
        }

        void null_device:: for_stdout()
        {
            YOCTO_LOCK(access);

#if defined(YOCTO_WIN)
            (void) freopen(YOCTO_NULL_DEVICE,"w",stdout);
#else
            stdout = freopen(YOCTO_NULL_DEVICE,"w",stdout);
#endif
            __check(stdout,"stdout");
        }

        void null_device:: for_cin()
        {
            
        }

        null_device:: null_device() :
        file_in(YOCTO_NULL_DEVICE),
        flag_in(false),
        buff_in(0),

        file_out(YOCTO_NULL_DEVICE),
        flag_out(false),
        buff_out(0),

        file_err(YOCTO_NULL_DEVICE),
        flag_err(false),
        buff_err(0)
        {
        }

        null_device:: ~null_device() throw()
        {
        }

        const char null_device::name[] = YOCTO_NULL_DEVICE;
    }

    
}

