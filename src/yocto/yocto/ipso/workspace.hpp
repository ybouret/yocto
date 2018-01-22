#ifndef YOCTO_IPSO_WORKSPACE_INCLUDED
#define YOCTO_IPSO_WORKSPACE_INCLUDED 1

#include "yocto/ipso/domain.hpp"
#include "yocto/ipso/xbuffer.hpp"
#include "yocto/ipso/field3d.hpp"
#include "yocto/associative/set.hpp"

namespace yocto
{
    namespace ipso
    {
        template <typename COORD>
        class workspace : public domain<COORD>, public counted
        {
        public:
            typedef field_info::pointer    field_ptr;
            typedef set<string,field_ptr>  field_db;

            static const size_t DIM = domain<COORD>::DIM;
            xbuffer::list xbuff[DIM];
            field_db      fields;

            //! create a domain with its ghosts
            /**
             \param block_size maximum block_size per coordinate for I/O
             */
            inline explicit workspace(const divider<COORD> &full,
                                      const size_t          rank,
                                      coord1D               ng,
                                      const COORD           pbcs,
                                      const size_t          block_size) :
            domain<COORD>(full,rank,ng,pbcs,true),
            xbuff(),
            fields(8,as_capacity)
            {
                // create a 1:1 xbuffers with async ghosts
                for(size_t dim=0;dim<DIM;++dim)
                {
                    for(const ghosts *G = this->async[dim].head;G;G=G->next)
                    {
                        const size_t count = G->count;
                        std::cerr << "in " << __coord_name(dim) << " new xbuffer(" << count << ") " << G->source << " <-> " << G->target << std::endl;
                        xbuff[dim].push_back( new xbuffer(count*block_size) );
                    }
                }
            }
            
            inline virtual ~workspace() throw()
            {
            }

            template <typename FIELD> inline
            FIELD & create( const string &field_name )
            {
                FIELD    *F = new FIELD( field_name, this->outer );
                field_ptr pF( F );
                if(!fields.insert(pF))
                {
                    throw exception("workspace%uD: multiple '%s'", unsigned(DIM), *field_name);
                }
                return *F;
            }

            template <typename FIELD> inline
            FIELD & create( const char *field_name )
            {
                const string id(field_name);
                return create<FIELD>(id);
            }
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(workspace);
        };
    }
}

#endif

