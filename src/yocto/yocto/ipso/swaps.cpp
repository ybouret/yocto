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

        bool swaps:: built() const throw()
        {
            assert(send.size()==recv.size());
            assert(send.size()==0||send.size()==count);
            return send.size()>0;
        }


        int swaps::compare_by_pos(const swaps *lhs, const swaps *rhs, void *) throw()
        {
            assert(lhs);
            assert(rhs);
            return int(lhs->pos)-int(rhs->pos);
        }

        static inline
        unsigned __swaps_pos2pos( const unsigned pos )
        {
            switch(pos)
            {
                case swaps::lower_x:
                case swaps::lower_y:
                case swaps::lower_z:
                    return swaps::lower_x;
                    
                case swaps::upper_x:
                case swaps::upper_y:
                case swaps::upper_z:
                    return swaps::upper_x;
                    
                default:
                    break;
            }
            return 0;
        }
        
        swaps * swaps:: clone1D() const
        {
            return new swaps(source,
                             target,
                             layers,
                             __swaps_pos2pos(pos));
        }
        
        swaps:: swaps(const swaps &other) :
        recv(other.recv),
        send(other.send),
        count(other.count),
        iobuf(),
        source(other.source),
        target(other.target),
        layers(other.layers),
        pos(other.pos),
        next(0),
        prev(0)
        {
        }
        
    }
}








