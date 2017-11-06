#ifndef YOCTO_PACK_XBWT_INCLUDED
#define YOCTO_PACK_XBWT_INCLUDED 1

#include "yocto/os.hpp"


namespace yocto
{

    namespace pack
    {

        struct xbwt
        {

            static size_t encode( void *output, const void *input, const size_t size);
            static void   decode( void *output, const void *input, const size_t size, const size_t primary_index);

        };

    }
}

#endif

