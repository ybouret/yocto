#ifndef YOCTO_IPSO_DOMAIN_INCLUDED
#define YOCTO_IPSO_DOMAIN_INCLUDED 1

#include "yocto/ipso/divide.hpp"
#include "yocto/ipso/ghosts.hpp"

namespace yocto
{
    namespace ipso
    {
        
        template <typename COORD>
        class domain : public object
        {
        public:
            typedef core::list_of_cpp<domain> list;
            typedef patch<COORD> patch_type;

            const COORD      ranks;
            const patch_type inner;
            const patch_type outer;

            ghosts::list     async; //!< async ghosts
            ghosts::list     local; //!< local ghosts

            domain          *next;
            domain          *prev;

            inline virtual ~domain() throw() {}

            explicit domain(const divider<COORD> &full,
                            const size_t          rank,
                            coord1D               ng,
                            const COORD           pbcs,
                            const bool            build);



        private:
            YOCTO_DISABLE_ASSIGN(domain);
        };

    }
}

#endif

