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

        void null_device:: for_cstdin()
        {
#if defined(YOCTO_WIN)
            (void) freopen(YOCTO_NULL_DEVICE,"r",stdin);
#else
            stdin = freopen(YOCTO_NULL_DEVICE,"r",stdin);
#endif
            __check(stdin,"stdin");
        }

        void null_device:: for_cstderr()
        {
#if defined(YOCTO_WIN)
            (void) freopen(YOCTO_NULL_DEVICE,"w",stderr);
#else
            stderr = freopen(YOCTO_NULL_DEVICE,"w",stderr);
#endif
            __check(stderr,"stderr");
        }

        void null_device:: for_cstdout()
        {
#if defined(YOCTO_WIN)
            (void) freopen(YOCTO_NULL_DEVICE,"w",stdout);
#else
            stdout = freopen(YOCTO_NULL_DEVICE,"w",stdout);
#endif
            __check(stdout,"stdout");
        }
    }

}

