
#ifndef YOCTO_IPSO_FIELD_INCLUDED
#define YOCTO_IPSO_FIELD_INCLUDED 1

#include "yocto/counted-object.hpp"
#include "yocto/ptr/intr.hpp"
#include "yocto/associative/set.hpp"
#include "yocto/ipso/swaps.hpp"

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

            const string name;  //!< unique identifier for set
            const size_t count; //!< items count, set by field after setup
            const size_t bytes; //!< bytes count, set by field after setup
            const size_t item_size; //!< one item size

            virtual ~field_info() throw();

            const string &key() const throw(); //!< name


            //! swap data locally
            virtual void swap_local(const swaps_list &G) throw() = 0;

            //! store data from send region
            virtual void swap_store(const swaps &G) const throw() = 0;

            //! query data from recv region
            virtual void swap_query(const swaps &G) const throw() = 0;


        protected:
            explicit field_info(const char   *id, const size_t itmsz);

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(field_info);
        };

        //! a field database is a set of field with access by their name
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

        enum field_ops
        {
            field_set,
            field_add,
            field_mul
        };

        //! With type, but dimension agnostic field
        template <typename T>
        class field : public field_info
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            static const size_t ITEM_SIZE = sizeof(T);

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

            //! debug by loading value into swap zone
            inline void swap_ops( const swap &swp, param_type value, field_ops op) throw()
            {
                assert(entry);
                for(size_t i=swp.size();i>0;--i)
                {
                    const coord1D j = swp[i]; assert(j>=0); assert(j<coord1D(count));
                    T &f = entry[j];
                    switch(op)
                    {
                        case field_set: f  = value; break;
                        case field_add: f += value; break;
                        case field_mul: f *= value; break;
                    }
                }
            }

            

            //! directly swap matching local swap pairs, without extra memory
            virtual void swap_local(const swaps_list &local) throw()
            {
                assert(0==(local.size&0x1)); // even number of local swaps
                if(local.size>0)
                {
                    T           *data = entry;
                    const swaps *a    = local.head; assert(a!=NULL);
                    for(size_t n=local.size>>1;n>0;--n)
                    {
                        const swaps *b = a->next;    assert(b!=NULL);
                        assert(a->count==b->count);
                        {
                            const string a_pos = a->flg2str(a->pos);
                            const string b_pos = b->flg2str(b->pos);
                            std::cerr << "local swap " << a_pos << " <-> " << b_pos << std::endl;
                        }

                        const array<coord1D> &a_send = a->send;
                        const array<coord1D> &a_recv = a->recv;
                        const array<coord1D> &b_send = b->send;
                        const array<coord1D> &b_recv = b->recv;

                        assert(a_send.size() == a->count );
                        assert(a_recv.size() == a->count );
                        assert(b_send.size() == b->count );
                        assert(b_recv.size() == b->count );

                        for(size_t g=a->count;g>0;--g)
                        {
                            assert(a_recv[g] < coord1D(this->count));
                            assert(a_send[g] < coord1D(this->count));
                            assert(b_recv[g] < coord1D(this->count));
                            assert(b_send[g] < coord1D(this->count));

                            data[ a_recv[g] ] = data[ b_send[g] ];
                            data[ b_recv[g] ] = data[ a_send[g] ];
                        }


                        a = b->next;
                    }
                }
            }

            //! store data from send region
            virtual void swap_store(const swaps &G) const throw()
            {
                assert(G.send.size()==G.count);
                for(size_t i=G.count;i>0;--i)
                {
                    const coord1D j=G.send[i];
                    assert(j<coord1D(count));
                    G.iobuf.store(entry[j]);
                }
            }

            //! query data from recv region
            virtual void swap_query(const swaps &G) const throw()
            {
                assert(G.recv.size()==G.count);
                for(size_t i=G.count;i>0;--i)
                {
                    const coord1D j=G.recv[i];
                    assert(j<coord1D(count));
                    G.iobuf.query(entry[j]);
                }
            }

            


        protected:
            inline explicit field(const char *id) :
            field_info(id,ITEM_SIZE), entry(0) {}
            
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


