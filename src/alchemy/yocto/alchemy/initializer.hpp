#ifndef YOCTO_ALCHEMY_INITIALIZER_INCLUDED
#define YOCTO_ALCHEMY_INITIALIZER_INCLUDED 1

#include "yocto/alchemy/equilibria.hpp"

namespace yocto
{
    namespace alchemy
    {

        struct initializer
        {
            void boot(equilibria &eqs, vector<double> &C0);
        };
    }
}

#endif
