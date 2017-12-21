#ifndef YOCTO_PRIMALITY_INCLUDED
#define YOCTO_PRIMALITY_INCLUDED 1

#include "yocto/os.hpp"

namespace yocto
{

    struct primality
    {
        static bool check0( const size_t n ) throw();
        static bool check1( const size_t n ) throw();
        static bool check2( const size_t n ) throw();
    };

}

#endif

