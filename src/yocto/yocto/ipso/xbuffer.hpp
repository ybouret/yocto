#ifndef YOCTO_IPSO_XBUFFER_INCLUDED
#define YOCTO_IPSO_XBUFFER_INCLUDED 1

#include "yocto/ipso/ghosts.hpp"
#include "yocto/counted-object.hpp"

namespace yocto
{
    namespace ipso
    {
        class exchange_buffer
        {
        public:

            exchange_buffer(const size_t num_bytes);
            ~exchange_buffer() throw();
            
            void   reset() throw();
            size_t load() const throw(); //!< wptr-rptr, #active bytes
            size_t left() const throw(); //!< last-wptr, #available bytes


            template <typename T>
            inline void store( const T &data ) throw()
            {
                assert(wptr<=last-sizeof(T));
                memcpy(wptr,&data,sizeof(T));
                wptr += sizeof(T);
            }
            
            template <typename T>
            inline T query() throw()
            {
                assert(rptr<=wptr-sizeof(T));
                T ans(0);
                memcpy(&ans,rptr,sizeof(T));
                rptr += sizeof(T);
                return ans;
            }
            
            template <typename T>
            inline void query( T &ans ) throw()
            {
                assert(rptr<=wptr-sizeof(T));
                memcpy(&ans,rptr,sizeof(T));
                rptr += sizeof(T);
            }

            //! artificial load, for I/O
            void load(const size_t input_bytes) throw();

            //! current read only area, with length=load
            const void *addr() const throw(); //!< first read only byte=rptr

            //! mimic memory exchange
            void copy(const exchange_buffer &from) throw();

        private:
            size_t         size; //!< total size in bytes
            uint8_t       *base; //!< memory base
            uint8_t       *rptr; //!< next address to query from
            uint8_t       *wptr; //!< next address to store into
            const uint8_t *last; //!< base+size, first invalid byte
            
            YOCTO_DISABLE_COPY_AND_ASSIGN(exchange_buffer);


        };

        //! a pair of exchange buffer
        class exchange_buffers : public object
        {
        public:
            typedef core::list_of_cpp<exchange_buffers> list;
            exchange_buffers    *next;
            exchange_buffers    *prev;
            exchange_buffer      send;
            exchange_buffer      recv;


            explicit exchange_buffers(const size_t bytes);
            virtual ~exchange_buffers() throw();

            void reset() throw();

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(exchange_buffers);
        };

    }
}

#endif

