#include "yocto/net/io/block.hpp"
#include "yocto/memory/global.hpp"
#include "yocto/code/utils.hpp"

#include <cstring>

namespace yocto
{
    namespace network
    {
        io_block:: ~io_block() throw()
        {
            memory::global::__free(blk);
        }

        io_block:: io_block( const size_t block_size) :
        next(0),
        prev(0),
        len( block_size ),
        blk( (uint8_t *)memory::global::__calloc(len, 1) ),
        r(blk),
        w(blk),
        last(blk+len)
        {
        }

        void io_block:: initialize() throw()
        {
            r=w=blk;
        }

#define Y_CHECK_NETBLK() \
assert(blk);   \
assert(r>=blk);\
assert(r<=last);\
assert(w>=r);  \
assert(w<=last)

        size_t io_block:: shift() const throw()
        {
            Y_CHECK_NETBLK();
            return static_cast<size_t>(r-blk);
        }

        size_t io_block:: bytes() const throw()
        {
            Y_CHECK_NETBLK();
            return static_cast<size_t>(w-r);
        }


        size_t io_block:: space() const throw()
        {
            Y_CHECK_NETBLK();
            return static_cast<size_t>(last-w);
        }


        void io_block:: defragment() throw()
        {
            Y_CHECK_NETBLK();

            if(r>blk)
            {
#ifndef NDEBUG
                const size_t old_bytes = bytes();
#endif
                memmove(blk,r,w-r);
                const size_t ns = static_cast<size_t>(r-blk);
                w -= ns;
                r  = blk;
                Y_CHECK_NETBLK();
                assert(bytes()==old_bytes);
            }
        }

        size_t io_block:: store(const void *data, const size_t size) throw()
        {
            assert(!(0==data&&size>0));
            const size_t nw = min_of(size,space());
            memcpy(w,data,nw);
            w += nw;
            Y_CHECK_NETBLK();
            return nw;
        }

        size_t io_block:: query(void *data, const size_t size) throw()
        {
            assert(!(0==data&&size>0));
            const size_t nr = min_of(size,bytes());
            memcpy(data,r,nr);
            r += nr;
            Y_CHECK_NETBLK();
            return nr;
        }


    }
}


#include "yocto/net/socket/io.hpp"
namespace yocto
{
    namespace network
    {

        size_t io_block:: recv( io_socket &iosock )
        {
            Y_CHECK_NETBLK();
            const size_t nr = iosock.recv(w,space());
            w += nr;
            Y_CHECK_NETBLK();
            return nr;
        }

        size_t io_block:: send( io_socket &iosock )
        {
            Y_CHECK_NETBLK();
            const size_t nw = iosock.send(r,bytes());
            r += nw;
            Y_CHECK_NETBLK();
            return nw;
        }
        
        
        
    }
    
}

