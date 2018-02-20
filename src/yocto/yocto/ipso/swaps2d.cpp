#include "yocto/ipso/swaps.hpp"

////////////////////////////////////////////////////////////////////////////////
//
// 2D
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
                        YOCTO_IPSO_SWAP_LOAD();
                        size_t indx = 1;
                        for(coord1D rx=outer.lower.x,sx=inner.lower.x,i=layers;i>0;--i,++rx,++sx)
                        {
                            _LOAD2D_Y();
                        }
                        io_check();
                    }
                    break;
                    
                case swaps::upper_x:
                    (size_t &)count = layers * inner.width.y;
                    if(build)
                    {
                        YOCTO_IPSO_SWAP_LOAD();
                        size_t indx = 1;
                        for(coord1D rx=outer.upper.x,sx=inner.upper.x,i=layers;i>0;--i,--rx,--sx)
                        {
                            _LOAD2D_Y();
                        }
                        io_check();
                    }
                    break;
                    
                    
                case swaps::lower_y:
                    (size_t &)count = layers * inner.width.x;
                    if(build)
                    {
                        YOCTO_IPSO_SWAP_LOAD();
                        size_t indx = 1;
                        for(coord1D ry=outer.lower.y,sy=inner.lower.y,j=layers;j>0;--j,++ry,++sy)
                        {
                            _LOAD2D_X();
                        }
                        io_check();
                    }
                    break;
                    
                case swaps::upper_y:
                    (size_t &)count = layers * inner.width.x;
                    if(build)
                    {
                        YOCTO_IPSO_SWAP_LOAD();
                        size_t indx = 1;
                        for(coord1D ry=outer.upper.y,sy=inner.upper.y,j=layers;j>0;--j,--ry,--sy)
                        {
                            _LOAD2D_X();
                        }
                        io_check();
                    }
                    break;
                    
                default:;
                    throw exception("swaps::load2D: unexpected swaps position '%s'", pos2txt(pos));
            }
        }
        
    }
    
}
