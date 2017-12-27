#ifndef YOCTO_GHOSTS_INCLUDED
#define YOCTO_GHOSTS_INCLUDED 1

#include "yocto/ipso/coord.hpp"
#include "yocto/sequence/vector.hpp"

namespace yocto
{
    namespace ipso
    {

        typedef vector<coord1D> ghost_indices;

        class ghost : public ghost_indices
        {
        public:
            explicit ghost() throw();
            virtual ~ghost() throw();
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(ghost);
        };

    }
}

#endif

