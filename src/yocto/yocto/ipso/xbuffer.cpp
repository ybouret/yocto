#include "yocto/ipso/xbuffer.hpp"
#include "yocto/memory/global.hpp"

namespace yocto
{
    namespace ipso
    {
        xbuffer:: ~xbuffer() throw()
        {
            memory::kind<memory::global>::release_as(base,size);
        }
        
        xbuffer:: xbuffer( const size_t num_bytes ) :
        size(num_bytes),
        base( memory::kind<memory::global>::acquire_as<uint8_t>(size) ),
        rptr(base),
        wptr(base),
        last( base+size ),
        next(0),
        prev(0)
        {
        }
        
        void xbuffer:: reset() throw()
        {
            rptr = base;
            wptr = base;
        }
        
        size_t xbuffer:: load() const throw()
        {
            return static_cast<size_t>(wptr-rptr);
        }
        
        size_t xbuffer:: left() const throw()
        {
            return static_cast<size_t>(last-wptr);
        }


        xbufferIO:: xbufferIO(const size_t bytes) :
        next(0),
        prev(0),
        send(bytes),
        recv(bytes)
        {
        }

        xbufferIO:: ~xbufferIO() throw()
        {
        }

        void xbufferIO:: reset() throw()
        {
            send.reset();
            recv.reset();
        }
    }
}
