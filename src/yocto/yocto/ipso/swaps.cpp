#include "yocto/ipso/swaps.hpp"

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

        void swaps:: allocate()
        {
            swap    & _recv = (swap &)recv;
            swap    & _send = (swap &)send;
            _send.make(count, 0);
            _recv.make(count, 0);
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

                    // 2D corner ghosts, 3D edge ghosts
                    __POS2TXT(lower_x|lower_y);
                    __POS2TXT(lower_x|upper_y);

                    __POS2TXT(upper_x|lower_y);
                    __POS2TXT(upper_x|upper_y);

            }
            return "?";
        }

    }
}


namespace yocto
{
    namespace ipso
    {

        void swaps:: load(const patch1D &inner,
                          const patch1D &outer,
                          const bool     build)
        {
            assert(layers>0);
            assert(0!=(pos&flag1D));
            (size_t &)count = layers;

            switch(pos)
            {
                case swaps::lower_x:
                    if(build)
                    {
                        allocate();
                        swap    & _recv = (swap &)recv;
                        swap    & _send = (swap &)send;
                        for(coord1D rx=outer.lower,sx=inner.lower,i=layers;i>0;--i,++rx,++sx)
                        {
                            _recv[i] = outer.offset_of(rx);
                            _send[i] = outer.offset_of(sx);
                        }
                    }
                    break;

                case swaps::upper_x:
                    if(build)
                    {
                        allocate();
                        swap    & _recv = (swap &)recv;
                        swap    & _send = (swap &)send;
                        for(coord1D rx=outer.upper,sx=inner.upper,i=layers;i>0;--i,--rx,--sx)
                        {
                            _recv[i] = outer.offset_of(rx);
                            _send[i] = outer.offset_of(sx);
                        }
                    }
                    break;
            }
        }


    }

}


