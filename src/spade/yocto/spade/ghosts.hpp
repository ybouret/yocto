#ifndef YOCTO_SPADE_GHOSTS_INCLUDED
#define YOCTO_SPADE_GHOSTS_INCLUDED 1

#include "yocto/spade/types.hpp"

namespace yocto
{
    namespace spade
    {

        class ghost
        {
        public:
            ghost();
            ~ghost() throw();

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(ghost);
        };

    }
}

#endif
