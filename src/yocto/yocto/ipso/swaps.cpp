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
                    
                    // 2D corner ghosts, 3D edge ghosts
                    __POS2TXT(lower_x|lower_y);
                    __POS2TXT(lower_x|upper_y);
                    
                    __POS2TXT(upper_x|lower_y);
                    __POS2TXT(upper_x|upper_y);
                    
            }
            return "?";
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
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// 1D
//
////////////////////////////////////////////////////////////////////////////////
namespace yocto
{
    namespace ipso
    {
        
#define _LOAD_PROLOG()          \
allocate();                     \
swap    & _recv = (swap &)recv; \
swap    & _send = (swap &)send;
        
#define _LOAD_EPILOG() assert(count==send.size()); assert(count==recv.size()); io_check()
        
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
                        _LOAD_PROLOG();
                        for(coord1D rx=outer.lower,sx=inner.lower,i=layers;i>0;--i,++rx,++sx)
                        {
                            _recv[i] = outer.offset_of(rx);
                            _send[i] = outer.offset_of(sx);
                        }
                        _LOAD_EPILOG();
                    }
                    break;
                    
                case swaps::upper_x:
                    if(build)
                    {
                        _LOAD_PROLOG();
                        for(coord1D rx=outer.upper,sx=inner.upper,i=layers;i>0;--i,--rx,--sx)
                        {
                            _recv[i] = outer.offset_of(rx);
                            _send[i] = outer.offset_of(sx);
                        }
                        _LOAD_EPILOG();
                    }
                    break;
                    
                default:
                    throw exception("swaps::load1D: unexpected swaps position '%s'", pos2txt(pos));
            }
        }
        
    }
    
}

////////////////////////////////////////////////////////////////////////////////
//
// 1D
//
////////////////////////////////////////////////////////////////////////////////
namespace yocto
{
    namespace ipso
    {
        
#define _LOAD2D_Y() do {                               \
for(coord1D y=inner.upper.y;y>=inner.lower.y;--y)      \
{                                                      \
{ coord2D r(rx,y); _recv[indx] = outer.offset_of(r); } \
{ coord2D s(sx,y); _send[indx] = outer.offset_of(s); } \
++indx;                                                \
} } while(false)
        
#define _LOAD2D_X() do {                                    \
for(coord1D x=inner.upper.x;x>=inner.lower.x;--x)           \
{                                                           \
{ const coord2D r(x,ry); _recv[indx] = outer.offset_of(r); }\
{ const coord2D s(x,sy); _send[indx] = outer.offset_of(s); }\
++indx;                                                     \
} } while(false)
        
        void swaps:: load(const patch2D &inner,
                          const patch2D &outer,
                          const bool     build)
        {
            assert(layers>0);
            switch(pos)
            {
                case swaps::lower_x:
                    (size_t &)count = layers * inner.width.y;
                    if(build)
                    {
                        _LOAD_PROLOG();
                        size_t indx = 1;
                        for(coord1D rx=outer.lower.x,sx=inner.lower.x,i=layers;i>0;--i,++rx,++sx)
                        {
                            _LOAD2D_Y();
                        }
                        _LOAD_EPILOG();
                    }
                    break;
                    
                case swaps::upper_x:
                    (size_t &)count = layers * inner.width.y;
                    if(build)
                    {
                        _LOAD_PROLOG();
                        size_t indx = 1;
                        for(coord1D rx=outer.upper.x,sx=inner.upper.x,i=layers;i>0;--i,--rx,--sx)
                        {
                            _LOAD2D_Y();
                        }
                        _LOAD_EPILOG();
                    }
                    break;
                    
                    
                case swaps::lower_y:
                    (size_t &)count = layers * inner.width.x;
                    if(build)
                    {
                        _LOAD_PROLOG();
                        size_t indx = 1;
                        for(coord1D ry=outer.lower.y,sy=inner.lower.y,j=layers;j>0;--j,++ry,++sy)
                        {
                            _LOAD2D_X();
                        }
                        _LOAD_EPILOG();
                    }
                    break;
                    
                case swaps::upper_y:
                    (size_t &)count = layers * inner.width.x;
                    if(build)
                    {
                        _LOAD_PROLOG();
                        size_t indx = 1;
                        for(coord1D ry=outer.upper.y,sy=inner.upper.y,j=layers;j>0;--j,--ry,--sy)
                        {
                            _LOAD2D_X();
                        }
                        _LOAD_EPILOG();
                    }
                    break;
                    
                default:;
                    throw exception("swaps::load2D: unexpected swaps position '%s'", pos2txt(pos));
            }
        }
        
    }
    
}




