#ifndef YOCTO_IOS_NULL_DEVICE_INCLUDED
#define YOCTO_IOS_NULL_DEVICE_INCLUDED 1

#include "yocto/threading/singleton.hpp"
#include <cstdio>
#include <fstream>

namespace yocto
{
    namespace ios
    {
#define YOCTO_NULL_DEVICE "/dev/null"
#if defined(YOCTO_WIN)
#undef YOCTO_NULL_DEVICE
#define YOCTO_NULL_DEVICE "NUL:"
#endif

#if 0
        struct null_device
        {
            static void for_cstdin();
            static void for_cstderr();
            static void for_cstdout();
        };
#endif

        class null_device : public singleton<null_device>
        {
        public:

            void for_stdin();
            void for_stdout();
            void for_stderr();

            void for_cin();

            static const char name[];
            static const threading::longevity life_time = 0;
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(null_device);
            friend class singleton<null_device>;

            explicit null_device();
            virtual ~null_device() throw();

            std::ifstream   file_in;
            bool            flag_in;
            std::streambuf *buff_in;

            std::ofstream   file_out;
            bool            flag_out;
            std::streambuf *buff_out;

            std::ofstream   file_err;
            bool            flag_err;
            std::streambuf *buff_err;

        };

    }

}


#endif

