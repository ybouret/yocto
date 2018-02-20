#include "yocto/ipso/swaps.hpp"

////////////////////////////////////////////////////////////////////////////////
//
// 1D
//
////////////////////////////////////////////////////////////////////////////////
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
                        YOCTO_IPSO_SWAP_LOAD();
                        for(coord1D rx=outer.lower,sx=inner.lower,i=layers;i>0;--i,++rx,++sx)
                        {
                            _recv[i] = outer.offset_of(rx);
                            _send[i] = outer.offset_of(sx);
                        }
                        io_check();
                    }
                    break;
                    
                case swaps::upper_x:
                    if(build)
                    {
                        YOCTO_IPSO_SWAP_LOAD();
                        for(coord1D rx=outer.upper,sx=inner.upper,i=layers;i>0;--i,--rx,--sx)
                        {
                            _recv[i] = outer.offset_of(rx);
                            _send[i] = outer.offset_of(sx);
                        }
                        io_check();
                    }
                    break;
                    
                default:
                    throw exception("swaps::load1D: unexpected swaps position '%s'", pos2txt(pos));
            }
        }
        
    }
    
}


