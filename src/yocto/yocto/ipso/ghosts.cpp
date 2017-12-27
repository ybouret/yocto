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

        ghosts:: ghosts(const coord1D d,
                        const coord1D n) throw() :
        send(),
        recv(),
        next(0),
        prev(0),
        dim(d),
        ng(n)
        {
            assert(ng>0);
        }

        static inline int __compare_coord1D(const coord1D lhs, const coord1D rhs) throw()
        {
            return lhs-rhs;
        }

        void ghosts:: load(const patch1D &inner, const patch1D &outer)
        {
            assert(ng>0);
            switch(dim)
            {
                case -1: {
                    send.make(ng);
                    recv.make(ng);
                    coord1D ix = inner.lower;
                    coord1D ox = outer.lower;
                    for(coord1D g=ng;g>0;--g,++ix,++ox)
                    {
                        const coord1D idx = outer.offset_of(ix);
                        const coord1D odx = outer.offset_of(ox);
                        send[g]=idx;
                        recv[g]=odx;
                    }
                }  break;

                case 1: {
                    send.make(ng);
                    recv.make(ng);
                    coord1D ix = inner.upper-ng+1;
                    coord1D ox = outer.upper-ng+1;
                    for(coord1D g=ng;g>0;--g,++ix,++ox)
                    {
                        const coord1D idx = outer.offset_of(ix);
                        const coord1D odx = outer.offset_of(ox);
                        send[g]=idx;
                        recv[g]=odx;
                    }
                } break;

                default:
                    throw exception("unexpected 1D ghosts.dim=%d", int(dim));
            }

            ySort(send,__compare_coord1D);
            ySort(recv,__compare_coord1D);


        }


    }
}
