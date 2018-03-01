#ifndef YOCTO_IPSO_SWAP_BUFFERS_INCLUDED
#define YOCTO_IPSO_SWAP_BUFFERS_INCLUDED 1

#include "yocto/container/cslot.hpp"

namespace yocto
{
    namespace ipso
    {

        class swap_buffers
        {
        public:
            const size_t bytes;

            ~swap_buffers() throw();
            swap_buffers() throw();

            void set_bytes( const size_t n );
            void release() throw();

            size_t in_send() const throw(); //!< current #send
            size_t in_recv() const throw(); //!< current #recv

            void reset() throw();

            template <typename T>
            inline void store( const T &data ) throw()
            {
                assert(bytes>0);
                assert(send_base);
                assert(send_curr>=send_base);
                assert(bytes==send_last-send_base);
                assert(send_curr+sizeof(T)<=send_last);
                memcpy(send_curr,&data,sizeof(T));
                send_curr += sizeof(T);
            }

            template <typename T>
            inline void query( T &data ) throw()
            {
                assert(bytes>0);
                assert(recv_base);
                assert(recv_curr>=recv_base);
                assert(bytes==recv_last-recv_base);
                assert(recv_curr+sizeof(T)<=recv_last);
                memcpy(&data,recv_curr,sizeof(T));
                recv_curr += sizeof(T);
            }

            //! set in_recv to in_send before I/O
            void prepare_recv() throw();
            

        private:
            uint8_t       *send_base;
            uint8_t       *send_curr;
            const uint8_t *send_last;
            uint8_t       *recv_base;
            const uint8_t *recv_curr;
            const uint8_t *recv_last;
            cslot          slot;
            YOCTO_DISABLE_COPY_AND_ASSIGN(swap_buffers);
            void clr() throw();
        };

    }
}

#endif

