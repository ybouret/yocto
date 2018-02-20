#include "yocto/ipso/swaps.hpp"

////////////////////////////////////////////////////////////////////////////////
//
// 3D
//
////////////////////////////////////////////////////////////////////////////////
namespace yocto
{
#define _LOAD3D_X()
    
    namespace ipso
    {
        static inline void __load3d_x(swap         &_recv,
                                      const coord1D rx,
                                      swap         &_send,
                                      const coord1D sx,
                                      const patch3D &inner,
                                      const patch3D &outer)
        {
            size_t indx = 0;
            for(coord1D z=inner.upper.z;z>=inner.lower.z;--z)
            {
                for(coord1D y=inner.upper.y;y>=inner.lower.y;--y)
                {
                    ++indx;
                    { const coord3D r(rx,y,z); _recv[indx] = outer.offset_of(r); }
                    { const coord3D s(sx,y,z); _send[indx] = outer.offset_of(s); }
                }
            }
        }
        
        static inline void __load3d_y(swap         &_recv,
                                      const coord1D ry,
                                      swap         &_send,
                                      const coord1D sy,
                                      const patch3D &inner,
                                      const patch3D &outer)
        {
            size_t indx = 0;
            for(coord1D z=inner.upper.z;z>=inner.lower.z;--z)
            {
                for(coord1D x=inner.upper.x;x>=inner.lower.x;--x)
                {
                    ++indx;
                    { const coord3D r(x,ry,z); _recv[indx] = outer.offset_of(r); }
                    { const coord3D s(x,sy,z); _send[indx] = outer.offset_of(s); }
                }
            }
        }
        
        static inline void __load3d_z(swap         &_recv,
                                      const coord1D rz,
                                      swap         &_send,
                                      const coord1D sz,
                                      const patch3D &inner,
                                      const patch3D &outer)
        {
            size_t indx = 0;
            for(coord1D y=inner.upper.y;y>=inner.lower.y;--y)
            {
                for(coord1D x=inner.upper.x;x>=inner.lower.x;--x)
                {
                    ++indx;
                    { const coord3D r(x,y,rz); _recv[indx] = outer.offset_of(r); }
                    { const coord3D s(x,y,rz); _send[indx] = outer.offset_of(s); }
                }
            }
        }
        
                                      
        void swaps:: load(const patch3D &inner,
                          const patch3D &outer,
                          const bool     build)
        {
            assert(layers>0);
            switch(pos)
            {
             
                case lower_x:
                    (size_t &)count = layers * inner.width.y*inner.width.z;
                    if(build)
                    {
                        YOCTO_IPSO_SWAP_LOAD();
                        for(coord1D rx=outer.lower.x,sx=inner.lower.x,i=layers;i>0;--i,++rx,++sx)
                        {
                            __load3d_x(_recv,rx,_send,sx,inner,outer);
                        }
                        io_check();
                    }
                    break;
                    
                    
                case upper_x:
                    (size_t &)count = layers * inner.width.y*inner.width.z;
                    if(build)
                    {
                        YOCTO_IPSO_SWAP_LOAD();
                        for(coord1D rx=outer.upper.x,sx=inner.upper.x,i=layers;i>0;--i,--rx,--sx)
                        {
                            __load3d_x(_recv,rx,_send,sx,inner,outer);
                        }
                        io_check();
                    }
                    break;
                    
                case lower_y:
                    (size_t &)count = layers * inner.width.x*inner.width.z;
                    if(build)
                    {
                        YOCTO_IPSO_SWAP_LOAD();
                        for(coord1D ry=outer.lower.y,sy=inner.lower.y,j=layers;j>0;--j,++ry,++sy)
                        {
                            __load3d_y(_recv,ry,_send,sy,inner,outer);
                        }
                        io_check();
                    }
                    break;
                    
                case upper_y:
                    (size_t &)count = layers * inner.width.x*inner.width.z;
                    if(build)
                    {
                        YOCTO_IPSO_SWAP_LOAD();
                        for(coord1D ry=outer.upper.y,sy=inner.upper.y,j=layers;j>0;--j,--ry,--sy)
                        {
                            __load3d_y(_recv,ry,_send,sy,inner,outer);
                        }
                        io_check();
                    }
                    break;
                    
                case lower_z:
                    (size_t &)count = layers * inner.width.x*inner.width.y;
                    if(build)
                    {
                        YOCTO_IPSO_SWAP_LOAD();
                        for(coord1D rz=outer.lower.z,sz=inner.lower.z,i=layers;i>0;--i,++rz,++sz)
                        {
                            __load3d_z(_recv,rz,_send,sz,inner,outer);
                        }
                        io_check();
                    }
                    break;
                    
                case upper_z:
                    (size_t &)count = layers * inner.width.x*inner.width.y;
                    if(build)
                    {
                        YOCTO_IPSO_SWAP_LOAD();
                        for(coord1D rz=outer.upper.z,sz=inner.upper.z,i=layers;i>0;--i,--rz,--sz)
                        {
                            __load3d_z(_recv,rz,_send,sz,inner,outer);
                        }
                        io_check();
                    }
                    break;
                
                    
            }
        }
    }
    
}


