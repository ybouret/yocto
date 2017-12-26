#ifndef YOCTO_PRIMALITY_INCLUDED
#define YOCTO_PRIMALITY_INCLUDED 1

#include "yocto/os.hpp"

namespace yocto
{

    struct primality
    {
        static bool   _check( const size_t n ) throw();
        static bool   check( const size_t n ) throw();
        static size_t prev(const size_t n);
        static size_t next(const size_t n);
    };

}

#endif

