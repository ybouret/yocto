#ifndef YOCTO_STRING_HEXDUMP_INCLUDED
#define YOCTO_STRING_HEXDUMP_INCLUDED 1

#include "yocto/string.hpp"
#include "yocto/ios/ostream.hpp"

namespace yocto
{
    struct hexdump
    {
        static void write( ios::ostream &fp, const char *data, const size_t size );
        static void write( ios::ostream &fp, const memory::ro_buffer &buff);
    };
}

#endif

