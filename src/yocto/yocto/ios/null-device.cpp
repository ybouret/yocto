#define _CRT_SECURE_NO_WARNINGS

#include "yocto/ios/null-device.hpp"
#include "yocto/exceptions.hpp"
#include <iostream>


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

#if defined(YOCTO_WIN) || defined(YOCTO_SUNOS) || defined(YOCTO_OPENBSD)
            (void) freopen(YOCTO_NULL_DEVICE,"r",stdin);
#else
            stdin = freopen(YOCTO_NULL_DEVICE,"r",stdin);
#endif
            __check(stdin,"stdin");
        }

        void null_device:: for_stderr()
        {
            YOCTO_LOCK(access);

#if defined(YOCTO_WIN) || defined(YOCTO_SUNOS) || defined(YOCTO_OPENBSD)
            (void) freopen(YOCTO_NULL_DEVICE,"w",stderr);
#else
            stderr = freopen(YOCTO_NULL_DEVICE,"w",stderr);
#endif
            __check(stderr,"stderr");
        }

        void null_device:: for_stdout()
        {
            YOCTO_LOCK(access);

#if defined(YOCTO_WIN) || defined(YOCTO_SUNOS) || defined(YOCTO_OPENBSD)
            (void) freopen(YOCTO_NULL_DEVICE,"w",stdout);
#else
            stdout = freopen(YOCTO_NULL_DEVICE,"w",stdout);
#endif
            __check(stdout,"stdout");
        }

        void null_device:: for_cin()
        {
            if(!flag_in)
            {
                flag_in = true;
                buff_in = std::cin.rdbuf( file_in.rdbuf() );
            }
        }


        void null_device:: for_cout()
        {
            if(!flag_out)
            {
                flag_out = true;
                buff_out = std::cout.rdbuf( file_out.rdbuf() );
            }
        }


        void null_device:: for_cerr()
        {
            if(!flag_err)
            {
                flag_err = true;
                buff_err = std::cerr.rdbuf( file_err.rdbuf() );
            }
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
            if(file_in.fail())
            {
                throw exception("couldn't open file_in(%s)",name);
            }

            if(file_out.fail())
            {
                throw exception("couldn't open file_out(%s)",name);
            }

            if(file_err.fail())
            {
                throw exception("couldn't open file_err(%s)",name);
            }
        }

        null_device:: ~null_device() throw()
        {
            if(flag_in)
            {
                std::cin.rdbuf( buff_in );
                flag_in = false;
            }

            if(flag_out)
            {
                std::cout.rdbuf( buff_out );
                flag_out = false;
            }

            if(flag_err)
            {
                std::cerr.rdbuf( buff_err );
                flag_err = false;
            }
        }

        const char null_device::name[] = YOCTO_NULL_DEVICE;
    }

    
}

