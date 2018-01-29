#ifndef YOCTO_INK_OPS_MAP_INCLUDED
#define YOCTO_INK_OPS_MAP_INCLUDED 1

#include "yocto/ink/parallel.hpp"
#include "yocto/ink/pixmap.hpp"

namespace yocto
{
    namespace Ink
    {
        //! one to one map
        class Map
        {
        public:
            template <typename T,typename U>
            static inline T Copy(const U &u) throw() { return T(u); }

            template <typename T,typename U,typename FUNC> inline
            void operator()(Pixmap<T>       &tgt,
                            const Pixmap<U> &src,
                            FUNC            &func,
                            const Domains   &doms)
            {
                target = &tgt;
                source = &src;
                proc   = (void*)&func;
                doms.enqueue(this, & Map::call<T,U,FUNC> );
            }

            Map() throw();
            virtual ~Map() throw();

        private:
            void       *target;
            const void *source;
            void       *proc;

            template <typename T,typename U,typename FUNC> inline
            void call( const Area &a, threading::context & ) throw()
            {
                const unit_t xmin = a.x;
                const unit_t xmax = a.x_end;
                const unit_t ymin = a.y;
                const unit_t ymax = a.y_end;
                Pixmap<T>       &tgt = *static_cast< Pixmap<T>       *>(target);
                const Pixmap<U> &src = *static_cast< const Pixmap<U> *>(source);
                FUNC            &func = *(FUNC *)proc;
                for(unit_t y=ymax;y>=ymin;--y)
                {
                    typename Pixmap<T>::Row       &Ty = tgt[y];
                    const typename Pixmap<U>::Row &Sy = src[y];
                    for(unit_t x=xmax;x>=xmin;--x)
                    {
                        Ty[x] = func(Sy[x]);
                    }
                }
            }
        };
    }
}

#endif

