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

#if 0
        swaps:: swaps(const swaps &lhs, const swaps &rhs) :
        recv(lhs.count+rhs.count),
        send(recv.size()),
        count(recv.size()),
        iobuf(),
        source(lhs.source),
        target(lhs.target),
        layers(lhs.layers),
        joined(true),
        pos(lhs.pos|rhs.pos),
        next(0),
        prev(0)
        {
            assert(!lhs.joined);
            assert(!rhs.joined);
            assert(lhs.source==rhs.source);
            assert(lhs.target==rhs.target);
            assert(lhs.count==rhs.count);
            assert(lhs.built());
            assert(rhs.built());
            std::cerr << "\t|_joining " << pos2txt(lhs.pos) << " and " << pos2txt(rhs.pos) << " to target " << target << std::endl;
#if 0
            std::cerr << "\t  |_" << pos2txt(lhs.pos) << ".send=" << lhs.send << std::endl;
            std::cerr << "\t  |_" << pos2txt(lhs.pos) << ".recv=" << lhs.recv << std::endl;
            std::cerr << "\t  |_" << pos2txt(rhs.pos) << ".send=" << rhs.send << std::endl;
            std::cerr << "\t  |_" << pos2txt(rhs.pos) << ".recv=" << rhs.recv << std::endl;
#endif
            const size_t com = lhs.count;
            for(size_t i=com;i>0;--i)
            {
                const size_t j=i+com;
                ((swap &)send)[i] = lhs.send[i];
                ((swap &)send)[j] = rhs.send[i];
                ((swap &)recv)[i] = rhs.recv[i];
                ((swap &)recv)[j] = lhs.recv[i];
            }
            io_check();
            std::cerr << "\t  |_send=" << send << std::endl;
            std::cerr << "\t  |_recv=" << recv << std::endl;
        }
#endif

    }
}








