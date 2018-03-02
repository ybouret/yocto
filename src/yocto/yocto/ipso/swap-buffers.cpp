#include "yocto/ipso/swap-buffers.hpp"
#include "yocto/code/round.hpp"


namespace yocto
{
    namespace ipso
    {
        swap_buffers:: ~swap_buffers() throw()
        {
        }

        swap_buffers:: swap_buffers() throw() :
        bytes(0),
        send_base(0),
        send_curr(0),
        send_last(0),
        recv_base(0),
        recv_curr(0),
        recv_last(0),
        slot(0)
        {
        }

        void swap_buffers:: clr() throw()
        {
            send_last = send_curr = send_base = 0;
            recv_last = recv_curr = recv_base = 0;
            (size_t&)bytes = 0;
        }

        void swap_buffers:: release() throw()
        {
            clr();
            slot.release();
        }

        void swap_buffers:: set_bytes( const size_t n )
        {
            clr();
            const size_t an = memory::align(n);
            const size_t nb = an*2;
            if(n>slot.size)
            {
                slot.release();
                cslot tmp(nb);
                slot.swap_with(tmp);
            }
            assert(slot.size>=nb);

            (size_t &)bytes = n;
            uint8_t *p = static_cast<uint8_t*>(slot.data);
            send_base  = &p[0];
            send_last  = send_base + bytes;
            recv_base  = &p[an];
            recv_last  = recv_base + bytes;

            reset();

        }

        size_t swap_buffers:: in_send() const throw()
        {
            return static_cast<size_t>(send_curr-send_base);
        }

        size_t swap_buffers:: in_recv() const throw()
        {
            return static_cast<size_t>(recv_last - recv_curr);
        }

        void swap_buffers:: reset() throw()
        {
            send_curr = send_base;
            recv_curr = recv_last;
        }

        void swap_buffers:: prepare_recv() throw()
        {
            recv_curr = recv_last - in_send();
        }

    }
}

