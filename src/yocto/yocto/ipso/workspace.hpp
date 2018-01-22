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
            static const size_t DIM = domain<COORD>::DIM;
            xbuffer::list xbuff[DIM];

            //! create a domain with its ghosts
            explicit workspace(const divider<COORD> &full,
                               const size_t          rank,
                               coord1D               ng,
                               const COORD           pbcs) :
            domain<COORD>(full,rank,ng,pbcs,true)
            {
                // create a 1:1 xbuffers with async ghosts
                for(size_t dim=0;dim<DIM;++dim)
                {
                    for(const ghosts *G = this->async[dim].head;G;G=G->next)
                    {
                        const size_t count = G->count;
                        std::cerr << "in " << __coord_name(dim) << " new xbuffer(" << count << ") " << G->source << " <-> " << G->target << std::endl;
                        xbuff[dim].push_back( new xbuffer(count) );
                    }
                }
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

