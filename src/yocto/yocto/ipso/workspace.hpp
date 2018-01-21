#ifndef YOCTO_IPSO_WORKSPACE_INCLUDED
#define YOCTO_IPSO_WORKSPACE_INCLUDED 1

#include "yocto/ipso/domain.hpp"
#include "yocto/ipso/xbuffer.hpp"

namespace yocto
{
    namespace ipso
    {
        template <typename COORD>
        class workspace : public domain<COORD>
        {
        public:
            //! create a domain with its ghosts
            explicit workspace(const divider<COORD> &full,
                               const size_t          rank,
                               coord1D               ng,
                               const COORD           pbcs) :
            domain<COORD>(full,rank,ng,pbcs,true)
            {
            }
            
            virtual ~workspace() throw()
            {
            }
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(workspace);
        };
    }
}

#endif

