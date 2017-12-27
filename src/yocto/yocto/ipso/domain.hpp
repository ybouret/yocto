#ifndef YOCTO_IPSO_DOMAIN_INCLUDED
#define YOCTO_IPSO_DOMAIN_INCLUDED 1

#include "yocto/ipso/divide.hpp"
#include "yocto/ipso/ghosts.hpp"

namespace yocto
{
    namespace ipso
    {
        
        template <typename COORD>
        class domain
        {
        public:
            typedef patch<COORD> patch_type;

            const COORD      ranks;
            const patch_type inner;
            const patch_type outer;

            inline virtual ~domain() throw() {}

            explicit domain(const divider<COORD> &full,
                            const size_t          rank,
                            const coord1D         ng,
                            const COORD           pbcs);



        private:
            YOCTO_DISABLE_ASSIGN(domain);
        };

    }
}

#endif

