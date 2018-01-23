
#ifndef YOCTO_IPSO_FIELD_INCLUDED
#define YOCTO_IPSO_FIELD_INCLUDED 1

#include "yocto/ipso/xbuffer.hpp"
#include "yocto/counted-object.hpp"
#include "yocto/ptr/intr.hpp"
#include "yocto/associative/set.hpp"


#include <cstring>

namespace yocto
{
    namespace ipso
    {

        //! dimension agnostic information about a field
        class field_info : public counted_object
        {
        public:
            typedef intr_ptr<string,field_info> pointer;
            typedef set<string,pointer>         set_type;

            const string name;  //!< unique identifier for database
            const size_t count; //!< items count, set by field after setup
            const size_t bytes; //!< bytes count, set by field after setup
            
            virtual ~field_info() throw();

            const string &key() const throw(); //!< name

            //! store data into xbuff.send
            virtual void store(const ghosts    &G,
                               exchange_buffer &xbuff ) const throw() = 0;

            //! fetch data from xbuff.recv
            virtual void query(const ghosts    &G,
                               exchange_buffer &xbuff) throw() = 0;

            //! exchange data locally
            virtual void local(const ghosts::list &G) throw() = 0;
            
        protected:
            explicit field_info(const char   *id);

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(field_info);
        };

        //! a field database is a set with access by key
        class field_db : public field_info::set_type
        {
        public:
            explicit field_db() throw();
            explicit field_db(const size_t n);
            virtual ~field_db() throw();
            field_info       & operator[](const string &id);
            const field_info & operator[](const string &id) const;
            field_info       & operator[](const char *id);
            const field_info & operator[](const char *id) const;
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(field_db);
        };

        //! With type, but dimension agnostic field
        template <typename T>
        class field : public field_info
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;

            inline virtual ~field() throw() {}

            T           *entry; //!< linear items


            //! set all to zero
            inline void ldz() throw() { assert(entry); assert(bytes); memset(entry,0,bytes); }

            //! set all to arg
            inline void ld( param_type arg ) throw()
            {
                assert(entry); assert(bytes);
                for(size_t i=0;i<count;++i)
                {
                    memcpy(&entry[i],&arg,sizeof(T));
                }

            }
            
            
            inline virtual void store(const ghosts    &G,
                                      exchange_buffer &xbuff ) const throw()
            {
                assert(G.send.size()==G.count);
                for(size_t i=G.count;i>0;--i)
                {
                    const coord1D j=G.send[i];
                    assert(j<coord1D(count));
                    xbuff.store(entry[j]);
                }
            }

            inline virtual void query(const ghosts    &G,
                                      exchange_buffer &xbuff) throw()
            {
                assert(G.recv.size()==G.count);
                for(size_t i=G.count;i>0;--i)
                {
                    const coord1D j=G.recv[i]; assert(j<coord1D(count));
                    xbuff.query(entry[j]);
                }
            }
            

            virtual void local(const ghosts::list &G) throw()
            {
                assert(2==G.size||0==G.size);
                if(G.size>0)
                {
                    const ghosts *a    = G.head;
                    const ghosts *b    = G.tail;
                    T            *data = entry;
                    assert(a!=b);
                    assert(a->count==b->count);
                    const array<coord1D> &a_send = a->send;
                    const array<coord1D> &a_recv = a->recv;
                    const array<coord1D> &b_send = b->send;
                    const array<coord1D> &b_recv = b->recv;

                    assert(a->send.size() == a->count );
                    assert(a->recv.size() == a->count );
                    assert(b->send.size() == b->count );
                    assert(b->recv.size() == b->count );
                    for(size_t g=a->count;g>0;--g)
                    {
                        assert(a_recv[g] < coord1D(this->count));
                        assert(a_send[g] < coord1D(this->count));
                        assert(b_recv[g] < coord1D(this->count));
                        assert(b_send[g] < coord1D(this->count));

                        data[ a_recv[g] ] = data[ b_send[g] ];
                        data[ b_recv[g] ] = data[ a_send[g] ];
                    }
                }
            }

        protected:
            inline explicit field(const char *id) :
            field_info(id), entry(0) {}
            
            void set_bytes(const size_t items) throw()
            {
                (size_t &)count = items;
                (size_t &)bytes = items*sizeof(T);
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(field);
        };

    }
}

#endif


