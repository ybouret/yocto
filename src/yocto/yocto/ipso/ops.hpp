#ifndef YOCTO_IPSO_OPS_INCLUDED
#define YOCTO_IPSO_OPS_INCLUDED 1

#include "yocto/ipso/patch.hpp"

namespace yocto
{
    namespace ipso
    {

        struct ops
        {
            template <
            typename TARGET,
            typename SOURCE,
            typename REGION>
            static inline
            void gradient( TARGET &target, const SOURCE &source, const REGION &region ) throw()
            {
                
            }
        };


    }
}

#endif

