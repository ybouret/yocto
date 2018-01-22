#ifndef YOCTO_IPSO_WORKSPACE_INCLUDED
#define YOCTO_IPSO_WORKSPACE_INCLUDED 1

#include "yocto/ipso/domain.hpp"
#include "yocto/ipso/xbuffer.hpp"
#include "yocto/ipso/field3d.hpp"

namespace yocto
{
    namespace ipso
    {
        template <typename COORD>
        class workspace : public domain<COORD>, public counted
        {
        public:

            static const size_t DIM = domain<COORD>::DIM;
            xbufferIO::list xbufIO[DIM];
            field_db        fields;

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
            xbufIO(),
            fields(8)
            {
                // create a 1:1 xbuffers with async ghosts
                for(size_t dim=0;dim<DIM;++dim)
                {
                    for(const ghosts *G = this->async[dim].head;G;G=G->next)
                    {
                        const size_t count = G->count;
                        std::cerr << "in " << __coord_name(dim) << " new xbuffer(" << count << ") " << G->source << " <-> " << G->target << std::endl;
                        xbufIO[dim].push_back( new xbufferIO(count*block_size) );
                    }
                }
            }
            
            inline virtual ~workspace() throw()
            {
            }

            template <typename FIELD> inline
            FIELD & create( const string &field_name )
            {
                FIELD                     *F = new FIELD( field_name, this->outer );
                const field_info::pointer  pF( F );
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



            inline bool owns( const field_info &F ) const throw()
            {
                for( field_db::const_iterator i=fields.begin();i!=fields.end();++i)
                {
                    const field_info &f = **i;
                    if( &f == &F ) return true;
                }
                return false;
            }



            //! initialize all the exchange buffers
            inline void sync_init() throw()
            {
                for(size_t dim=0;dim<DIM;++dim)
                {
                    for(xbufferIO *pX = this->xbufIO[dim].head;pX;pX=pX->next)
                    {
                        pX->reset();
                    }
                }
            }

            //! store all data, them ready for I/O
            /**
             - perform local exchange
             - store data to be sent in exchange buffers
             */
            inline void sync_store( field_info &F ) throw()
            {
                assert(owns(F));
                for(size_t dim=0;dim<DIM;++dim)
                {
                    F.local( this->local[dim] );
                    const ghosts     *pG = this->async[dim].head;
                    xbufferIO        *pX = this->xbufIO[dim].head;
                    for(;pG;pG=pG->next,pX=pX->next)
                    {
                        assert(pX);
                        F.store(*pG,pX->send);
                    }
                }
            }

            

            //! query all data from exchange buffers after I/O
            inline void sync_query( field_info &F ) throw()
            {
                assert(owns(F));
                for(size_t dim=0;dim<DIM;++dim)
                {
                    const ghosts     *pG = this->async[dim].head;
                    xbufferIO        *pX = this->xbufIO[dim].head;
                    for(;pG;pG=pG->next,pX=pX->next)
                    {
                        assert(pX);
                        F.query(*pG,pX->recv);
                    }
                }
            }
            
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(workspace);
        };
    }
}

#endif

