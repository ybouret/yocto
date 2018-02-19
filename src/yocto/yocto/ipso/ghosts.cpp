#include "yocto/ipso/ghosts.hpp"
#include "yocto/exception.hpp"
#include "yocto/sort/ysort.hpp"

namespace yocto
{
    namespace ipso
    {
        ghosts:: ~ ghosts() throw()
        {
        }

        ghosts:: ghosts(const unsigned  where,
                        const coord1D   extent,
                        const size_t    the_source,
                        const size_t    the_target) throw() :
        send(),
        recv(),
        count(0),
        source(the_source),
        target(the_target),
        source_index(source+1),
        target_index(target+1),
        next(0),
        prev(0),
        location(where),
        ng(extent)
        {
            assert(ng>0);
        }

#if 1
        unsigned ghosts::dim2pos(const size_t dim, const int s)
        {

            if(dim==0)
            {
                if(s==-1) return at_lower_x;
                if(s== 1) return at_upper_x;
            }

            if(dim==1)
            {
                if(s==-1) return at_lower_y;
                if(s== 1) return at_upper_y;
            }

            if(dim==2)
            {
                if(s==-1) return at_lower_z;
                if(s== 1) return at_upper_z;
            }

            throw exception("ghosts::dim2pos: invalid dim=%u and sign=%d", unsigned(dim), int(s));
        }
#endif

#define POS2TXT(ID) case ID: return #ID
        const char * ghosts:: pos2txt(const unsigned p) throw()
        {
            switch(p)
            {
                    POS2TXT(at_lower_x);
                    POS2TXT(at_upper_x);
                    POS2TXT(at_lower_y);
                    POS2TXT(at_upper_y);
                    POS2TXT(at_lower_z);
                    POS2TXT(at_upper_z);
            }
            return "unknown";
        }
        
        static inline int __compare_coord1D(const coord1D lhs, const coord1D rhs) throw()
        {
            return lhs-rhs;
        }

#define __LOAD_PROLOG()           \
assert(ng>0);                     \
ghost  & _send  = (ghost  &)send; \
ghost  & _recv  = (ghost  &)recv; \
size_t & _count = (size_t &)count

#define __LOAD_EPILOG()         \
if(build) {                     \
ySort(_send,__compare_coord1D); \
ySort(_recv,__compare_coord1D); \
}

#define __LOAD_ALLOC() \
_send.make(_count);    \
_recv.make(_count)

        ////////////////////////////////////////////////////////////////////////
        //
        // 1D
        //
        ////////////////////////////////////////////////////////////////////////
        void ghosts:: load(const patch1D &inner,
                           const patch1D &outer,
                           const bool     build)
        {
            __LOAD_PROLOG();
            switch(location)
            {
                case at_lower_x: {
                    _count = ng;
                    if(build)
                    {
                        __LOAD_ALLOC();
                        coord1D ix = inner.lower;
                        coord1D ox = outer.lower;
                        for(coord1D g=ng;g>0;--g,++ix,++ox)
                        {
                            const coord1D idx = outer.offset_of(ix);
                            const coord1D odx = outer.offset_of(ox);
                            _send[g]=idx;
                            _recv[g]=odx;
                        }
                    }
                }  break;

                case at_upper_x: {
                    _count = ng;
                    if(build)
                    {
                        __LOAD_ALLOC();
                        coord1D ix = inner.upper-ng;
                        coord1D ox = outer.upper-ng;
                        for(coord1D g=ng;g>0;--g)
                        {
                            const coord1D idx = outer.offset_of(++ix);
                            const coord1D odx = outer.offset_of(++ox);
                            _send[g]=idx;
                            _recv[g]=odx;
                        }
                    }
                } break;

                default:
                    throw exception("unexpected 1D ghosts %s", pos2txt(location));
            }

            __LOAD_EPILOG();

        }


        ////////////////////////////////////////////////////////////////////////
        //
        // 2D
        //
        ////////////////////////////////////////////////////////////////////////
        void ghosts:: load(const patch2D &inner,
                           const patch2D &outer,
                           const bool     build)
        {
            __LOAD_PROLOG();
            switch(location)
            {
                case at_lower_x :
                {
                    _count = ng * outer.width.y;
                    if(build)
                    {
                        __LOAD_ALLOC();
                        coord1D ix = inner.lower.x;
                        coord1D ox = outer.lower.x;
                        size_t  ig = 0;
                        for(coord1D g=ng;g>0;--g,++ix,++ox)
                        {
                            for(coord1D j=outer.lower.y;j<=outer.upper.y;++j)
                            {
                                const coord2D icoord(ix,j);
                                const coord2D ocoord(ox,j);
                                ++ig;
                                _send[ig] = outer.offset_of(icoord);
                                _recv[ig] = outer.offset_of(ocoord);
                            }
                        }
                        assert(count==ig);
                    }
                } break;

                case at_upper_x :
                {
                    _count = ng * outer.width.y;
                    if(build)
                    {
                        __LOAD_ALLOC();
                        coord1D ix = inner.upper.x-ng;
                        coord1D ox = outer.upper.x-ng;
                        size_t  ig = 0;
                        for(coord1D g=ng;g>0;--g)
                        {
                            ++ix;
                            ++ox;
                            for(coord1D j=outer.lower.y;j<=outer.upper.y;++j)
                            {
                                const coord2D icoord(ix,j);
                                const coord2D ocoord(ox,j);
                                ++ig;
                                _send[ig] = outer.offset_of(icoord);
                                _recv[ig] = outer.offset_of(ocoord);
                            }
                        }
                        assert(count==ig);
                    }
                } break;

                case at_lower_y :
                {
                    _count = ng * outer.width.x;
                    if(build)
                    {
                        __LOAD_ALLOC();
                        coord1D iy = inner.lower.y;
                        coord1D oy = outer.lower.y;
                        size_t  ig = 0;
                        for(coord1D g=ng;g>0;--g,++iy,++oy)
                        {
                            for(coord1D i=outer.lower.x;i<=outer.upper.x;++i)
                            {
                                const coord2D icoord(i,iy);
                                const coord2D ocoord(i,oy);
                                ++ig;
                                _send[ig] = outer.offset_of(icoord);
                                _recv[ig] = outer.offset_of(ocoord);
                            }
                        }
                        assert(count==ig);
                    }
                } break;

                case at_upper_y :
                {
                    _count = ng * outer.width.x;
                    if(build)
                    {
                        __LOAD_ALLOC();
                        coord1D iy = inner.upper.y-ng;
                        coord1D oy = outer.upper.y-ng;
                        size_t  ig = 0;
                        for(coord1D g=ng;g>0;--g)
                        {
                            ++iy;
                            ++oy;
                            for(coord1D i=outer.lower.x;i<=outer.upper.x;++i)
                            {
                                const coord2D icoord(i,iy);
                                const coord2D ocoord(i,oy);
                                ++ig;
                                _send[ig] = outer.offset_of(icoord);
                                _recv[ig] = outer.offset_of(ocoord);
                            }
                        }
                        assert(count==ig);
                    }
                } break;


                default:
                    throw exception("unexpected 2D ghosts %s", pos2txt(location));
            }
            __LOAD_EPILOG();
        }


        ////////////////////////////////////////////////////////////////////////
        //
        // 3D
        //
        ////////////////////////////////////////////////////////////////////////

        void ghosts:: load(const patch3D &inner,
                           const patch3D &outer,
                           const bool     build)
        {
            __LOAD_PROLOG();
            switch(location)
            {
                case at_lower_x:
                {
                    _count = ng * outer.width.y*outer.width.z;
                    if(build)
                    {
                        __LOAD_ALLOC();
                        coord1D ix = inner.lower.x;
                        coord1D ox = outer.lower.x;
                        size_t  ig = 0;
                        for(coord1D g=ng;g>0;--g,++ix,++ox)
                        {
                            for(coord1D j=outer.lower.y;j<=outer.upper.y;++j)
                            {
                                for(coord1D k=outer.lower.z;k<=outer.upper.z;++k)
                                {
                                    const coord3D icoord(ix,j,k);
                                    const coord3D ocoord(ox,j,k);
                                    ++ig;
                                    _send[ig] = outer.offset_of(icoord);
                                    _recv[ig] = outer.offset_of(ocoord);
                                }
                            }
                        }
                        assert(count==ig);
                    }
                } break;

                case at_upper_x:
                {
                    _count = ng * outer.width.y*outer.width.z;
                    if(build)
                    {
                        __LOAD_ALLOC();
                        coord1D ix = inner.upper.x-ng;
                        coord1D ox = outer.upper.x-ng;
                        size_t  ig = 0;
                        for(coord1D g=ng;g>0;--g)
                        {
                            ++ix;
                            ++ox;
                            for(coord1D j=outer.lower.y;j<=outer.upper.y;++j)
                            {
                                for(coord1D k=outer.lower.z;k<=outer.upper.z;++k)
                                {
                                    const coord3D icoord(ix,j,k);
                                    const coord3D ocoord(ox,j,k);
                                    ++ig;
                                    _send[ig] = outer.offset_of(icoord);
                                    _recv[ig] = outer.offset_of(ocoord);
                                }
                            }
                        }
                        assert(count==ig);
                    }
                } break;

                case at_lower_y :
                {
                    _count = ng * outer.width.x*outer.width.z;
                    if(build)
                    {
                        __LOAD_ALLOC();
                        coord1D iy = inner.lower.y;
                        coord1D oy = outer.lower.y;
                        size_t  ig = 0;
                        for(coord1D g=ng;g>0;--g,++iy,++oy)
                        {
                            for(coord1D i=outer.lower.x;i<=outer.upper.x;++i)
                            {
                                for(coord1D k=outer.lower.z;k<=outer.upper.z;++k)
                                {
                                    const coord3D icoord(i,iy,k);
                                    const coord3D ocoord(i,oy,k);
                                    ++ig;
                                    _send[ig] = outer.offset_of(icoord);
                                    _recv[ig] = outer.offset_of(ocoord);
                                }
                            }
                        }
                        assert(count==ig);
                    }
                } break;

                case at_upper_y :
                {
                    _count = ng * outer.width.x*outer.width.z;
                    if(build)
                    {
                        __LOAD_ALLOC();
                        coord1D iy = inner.upper.y-ng;
                        coord1D oy = outer.upper.y-ng;
                        size_t  ig = 0;
                        for(coord1D g=ng;g>0;--g)
                        {
                            ++iy;
                            ++oy;
                            for(coord1D i=outer.lower.x;i<=outer.upper.x;++i)
                            {
                                for(coord1D k=outer.lower.z;k<=outer.upper.z;++k)
                                {
                                    const coord3D icoord(i,iy,k);
                                    const coord3D ocoord(i,oy,k);
                                    ++ig;
                                    _send[ig] = outer.offset_of(icoord);
                                    _recv[ig] = outer.offset_of(ocoord);
                                }
                            }
                        }
                        assert(count==ig);
                    }
                } break;


                case at_lower_z :
                {
                    _count = ng * outer.width.x*outer.width.y;
                    if(build)
                    {
                        __LOAD_ALLOC();
                        coord1D iz = inner.lower.z;
                        coord1D oz = outer.lower.z;
                        size_t  ig = 0;
                        for(coord1D g=ng;g>0;--g,++iz,++oz)
                        {
                            for(coord1D i=outer.lower.x;i<=outer.upper.x;++i)
                            {
                                for(coord1D j=outer.lower.y;j<=outer.upper.y;++j)
                                {
                                    const coord3D icoord(i,j,iz);
                                    const coord3D ocoord(i,j,oz);
                                    ++ig;
                                    _send[ig] = outer.offset_of(icoord);
                                    _recv[ig] = outer.offset_of(ocoord);
                                }
                            }
                        }
                        assert(count==ig);
                    }
                } break;

                case at_upper_z :
                {
                    _count = ng * outer.width.x*outer.width.y;
                    if(build)
                    {
                        __LOAD_ALLOC();
                        coord1D iz = inner.upper.z-ng;
                        coord1D oz = outer.upper.z-ng;
                        size_t  ig = 0;
                        for(coord1D g=ng;g>0;--g)
                        {
                            ++iz;
                            ++oz;
                            for(coord1D i=outer.lower.x;i<=outer.upper.x;++i)
                            {
                                for(coord1D j=outer.lower.y;j<=outer.upper.y;++j)
                                {
                                    const coord3D icoord(i,j,iz);
                                    const coord3D ocoord(i,j,oz);
                                    ++ig;
                                    _send[ig] = outer.offset_of(icoord);
                                    _recv[ig] = outer.offset_of(ocoord);
                                }
                            }
                        }
                        assert(count==ig);
                    }
                } break;

                default:
                    throw exception("unexpected 3D ghosts %s", pos2txt(location));
            }
            __LOAD_EPILOG();
        }


    }
}
