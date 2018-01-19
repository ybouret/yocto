#include "yocto/ipso/xbuffer.hpp"
#include "yocto/memory/global.hpp"

namespace yocto
{
    namespace ipso
    {
        exchange_buffer:: ~exchange_buffer() throw()
        {
        }
        
        exchange_buffer:: exchange_buffer( const size_t num_bytes ) :
        size(num_bytes),
        base( memory::kind<memory::global>::acquire_as<uint8_t>(size) ),
        rptr(base),
        wptr(base),
        last( base+size )
        {
        }
        
        void exchange_buffer:: reset() throw()
        {
            rptr = base;
            wptr = base;
        }
        
        size_t exchange_buffer:: load() const throw()
        {
            return static_cast<size_t>(wptr-rptr);
        }
        
        size_t exchange_buffer:: left() const throw()
        {
            return static_cast<size_t>(last-wptr);
        }

    }
}
