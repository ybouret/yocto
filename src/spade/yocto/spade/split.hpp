#ifndef YOCTO_SPADE_SPLIT_INCLUDED
#define YOCTO_SPADE_SPLIT_INCLUDED 1

#include "yocto/spade/layout.hpp"

namespace yocto
{
    namespace spade
    {
        struct split_ops
        {
            static void basic(const size_t size,
                              const size_t rank,
                              coord1D     &offset,
                              coord1D     &length);

            static Layout1D in1D(const size_t    size,
                                 const size_t    rank,
                                 const Layout1D &full);
        };
    }
}


#endif
