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

        ghosts:: ghosts(const position  the_pos,
                        const coord1D   the_ng,
                        const size_t    the_source,
                        const size_t    the_target) throw() :
        send(),
        recv(),
        count(0),
        source(the_source),
        target(the_target),
        next(0),
        prev(0),
        pos(the_pos),
        ng(the_ng)
        {
            assert(ng>0);
        }

        ghosts::position ghosts::dim2pos(const size_t dim, const int s)
        {

            if(dim==0)
            {
                if(s==-1) return x_lower;
                if(s== 1) return x_upper;
            }

            if(dim==1)
            {
                if(s==-1) return y_lower;
                if(s== 1) return y_upper;
            }

            if(dim==2)
            {
                if(s==-1) return z_lower;
                if(s== 1) return z_upper;
            }

            throw exception("ghosts::dim2pos: invalid dim=%u and sign=%d", unsigned(dim), int(s));
        }


        static inline int __compare_coord1D(const coord1D lhs, const coord1D rhs) throw()
        {
            return lhs-rhs;
        }

        void ghosts:: load(const patch1D &inner,
                           const patch1D &outer,
                           const bool     build)
        {
            assert(ng>0);
            ghost  & _send  = (ghost &)send;
            ghost  & _recv  = (ghost &)recv;
            size_t & _count = (size_t &)count;
            switch(pos)
            {
                case x_lower: {
                    _count = ng;
                    if(build)
                    {
                        _send.make(_count);
                        _recv.make(_count);
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

                case x_upper: {
                    _count = ng;
                    if(build)
                    {
                        _send.make(_count);
                        _recv.make(_count);
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
                    throw exception("unexpected 1D ghosts direction");
            }

            if(build)
            {
                ySort(_send,__compare_coord1D);
                ySort(_recv,__compare_coord1D);
            }

        }


    }
}
