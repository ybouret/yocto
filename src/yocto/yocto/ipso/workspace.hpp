#ifndef YOCTO_IPSO_WORKSPACE_INCLUDED
#define YOCTO_IPSO_WORKSPACE_INCLUDED 1

#include "yocto/ipso/fields.hpp"
#include "yocto/ipso/subset.hpp"

namespace yocto
{
    namespace ipso
    {

        template <typename COORD>
        class workspace : public subset<COORD>, public field_db, public counted
        {
        public:
            static const size_t    DIM = YOCTO_IPSO_DIM_OF(COORD);

            //! create a subset with its swaps
            inline explicit workspace(const divider<COORD> &full,
                                      const size_t          rank,
                                      const size_t          layers,
                                      const COORD           pbcs) :
            subset<COORD>(full,rank,layers,pbcs,true),
            field_db(8),
            counted()
            {
            }

            //! default dtor
            inline virtual ~workspace() throw() {}

            //! helper to create a field
            template <typename FIELD>
            inline FIELD & create( const string &field_name )
            {
                return this->template build<FIELD>(field_name,this->outer);
            }

            //! helper to create a field
            template <typename FIELD>
            inline FIELD & create( const char *field_name )
            {
                return this->template build<FIELD>(field_name,this->outer);
            }
            

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(workspace);
        };

#if 0
        //! a workspace is a set of fields on a domain
        template <typename COORD>
        class workspace : public domain<COORD>, public field_db, public counted
        {
        public:

            static const size_t    DIM = domain<COORD>::DIM;
            exchange_buffers::list iobuf[DIM];
            
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
            field_db(8),
            iobuf()
            {
                // create a 2:1 exchange_buffer with async ghosts
                for(size_t dim=0;dim<DIM;++dim)
                {
                    for(const ghosts *G = this->async[dim].head;G;G=G->next)
                    {
                        iobuf[dim].push_back( new exchange_buffers(G->count*block_size) );
                    }
                }
            }
            
            //! destructor, nothing special to do
            inline virtual ~workspace() throw() {}

            //! helper to create a field
            template <typename FIELD>
            inline FIELD & create( const string &field_name )
            {
                FIELD                     *F = new FIELD( field_name, this->outer );
                const field_info::pointer  pF( F );
                if(!this->insert(pF))
                {
                    throw exception("workspace%uD: multiple '%s'", unsigned(DIM), *field_name);
                }
                return *F;
            }

            //! wrapper
            template <typename FIELD>
            inline FIELD & create( const char *field_name )
            {
                const string id(field_name);
                return create<FIELD>(id);
            }
            

            inline bool owns( const field_info &F ) const throw()
            {
                for( field_db::const_iterator i=this->begin();i!=this->end();++i)
                {
                    const field_info &f = **i;
                    if( &f == &F ) return true;
                }
                return false;
            }

            //! initialize all the exchange buffers
            inline void sync_store_begin() throw()
            {
                for(size_t dim=0;dim<DIM;++dim)
                {
                    for(exchange_buffers *B = iobuf[dim].head;B;B=B->next)
                    {
                        B->reset();
                    }
                }
            }

            //! store all data from ONE field, then ready for I/O
            /**
             - perform local exchange
             - store data to be sent in exchange buffers
             */
            inline void sync_store( field_info &F ) throw()
            {
                assert(owns(F));
                for(size_t dim=0;dim<DIM;++dim)
                {
                    F.local_exchange( this->local[dim] );
                    const ghosts      *G = this->async[dim].head;
                    exchange_buffers  *B = this->iobuf[dim].head;
                    for(;G;G=G->next,B=B->next)
                    {
                        assert(B);
                        F.store(*G,B->send);
                    }
                }
            }

            //! store all data from fields
            inline void sync_store( fields &fvar ) throw()
            {
                for(size_t i=fvar.size();i>0;--i)
                {
                    sync_store( *fvar[i] );
                }
            }

            //! prepare recv buffers once all the send buffers are filled
            inline void sync_store_end() throw()
            {
                for(size_t dim=0;dim<DIM;++dim)
                {
                    for(exchange_buffers *B = this->iobuf[dim].head;B;B=B->next)
                    {
                        B->recv.load( B->send.load() );
                    }
                }
            }
            
            

            //! query all data for ONE field from exchange buffers after I/O
            inline void sync_query( field_info &F ) throw()
            {
                assert(owns(F));
                for(size_t dim=0;dim<DIM;++dim)
                {
                    const ghosts     *G = this->async[dim].head;
                    exchange_buffers *B = this->iobuf[dim].head;
                    for(;G;G=G->next,B=B->next)
                    {
                        assert(B);
                        F.query(*G,B->recv);
                    }
                }
            }

            inline void sync_query( fields &fvar ) throw()
            {
                for(size_t i=fvar.size();i>0;--i)
                {
                    sync_query( *fvar[i] );
                }
            }
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(workspace);
        };
#endif
    }
}

#endif

