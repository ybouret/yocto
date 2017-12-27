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

        ghosts:: ghosts(const direction the_dir,
                        const coord1D   the_ng,
                        const size_t    the_source,
                        const size_t    the_target) throw() :
        send(),
        recv(),
        source(the_source),
        target(the_target),
        next(0),
        prev(0),
        dir(the_dir),
        ng(the_ng)
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
            switch(dir)
            {
                case x_lower: {
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

                case x_upper: {
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
                    throw exception("unexpected 1D ghosts direction");
            }

            ySort(send,__compare_coord1D);
            ySort(recv,__compare_coord1D);


        }


    }
}
