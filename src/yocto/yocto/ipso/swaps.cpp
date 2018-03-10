#include "yocto/ipso/swaps.hpp"
#include "yocto/sort/heap.hpp"

namespace yocto
{
    namespace ipso
    {
        
        swaps:: ~swaps() throw()
        {
        }
        
        swaps:: swaps(const size_t   source_rank,
                      const size_t   target_rank,
                      const size_t   some_layers,
                      const unsigned where) throw() :
        recv(),
        send(),
        count(0),
        iobuf(),
        source(source_rank),
        target(target_rank),
        layers(some_layers),
        pos(where),
        next(0),
        prev(0)
        {
            assert(layers>0);
        }
        
        unsigned swaps:: dim2pos( const size_t dim, const int side ) throw()
        {
            assert(dim<3);
            assert(1==side||-1==side);
            return unsigned(1) << unsigned((dim<<1)+(unsigned(1+side)>>1));
        }
        
        
        
#define __POS2TXT(FLAG) case FLAG: return #FLAG
        
        const char * swaps:: pos2txt( const unsigned flag ) throw()
        {
            switch(flag)
            {
                    // straight ghosts
                    __POS2TXT(lower_x); // 1D/2D/3D
                    __POS2TXT(upper_x); // 1D/2D/3D
                    
                    __POS2TXT(lower_y); // 2D/3D
                    __POS2TXT(upper_y); // 2D/3D
                    
                    __POS2TXT(lower_z); // 3D
                    __POS2TXT(upper_z); // 3D

                default: break;
                    
            }
            return "?";
        }

        string swaps:: flg2str( const unsigned flags)
        {
            string ans;
            ans << '[';
            for(int i=0;i<6;++i)
            {
                const unsigned p = flags & (1 << i);
                if(p)
                {
                    ans << ' ' << pos2txt(p);
                }
            }
            ans << ' ' << ']';
            return ans;
        }
        
        void swaps:: allocate()
        {
            swap    & _recv = (swap &)recv;
            swap    & _send = (swap &)send;
            _send.make(count, 0);
            _recv.make(count, 0);
        }
        
        void swaps:: io_check() throw()
        {
            swap    & _recv = (swap &)recv;
            swap    & _send = (swap &)send;
            hsort(_recv, _send, __compare_decreasing<coord1D> );
        }

        void swaps:: allocate_for(const size_t block_size)
        {
            iobuf.set_bytes( count * block_size );
        }

        void swaps:: join( const swaps &other )
        {
            assert(this!=&other);
            swap    & _recv = (swap &)recv;
            swap    & _send = (swap &)send;
            const size_t total_count = count + other.count;
            _recv.ensure(total_count);
            _send.ensure(total_count);
            for(size_t i=1;i<=other.count;++i)
            {
                _recv.__push_back(other.recv[i]);
                _send.__push_back(other.send[i]);
            }
            assert(recv.size()==total_count);
            assert(send.size()==total_count);
            (size_t &)count = total_count;
            // TODO: io_check
        }
    }
}








