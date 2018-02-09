#ifndef YOCTO_INK_OPS_MAPPER_INCLUDED
#define YOCTO_INK_OPS_MAPPER_INCLUDED 1

#include "yocto/ink/parallel.hpp"
#include "yocto/ink/pixmap.hpp"

namespace yocto
{
    namespace Ink
    {
        //! one to one pixel mapping functions
        class Mapper
        {
        public:
           

            template <typename T,typename U,typename FUNC> inline
            void operator()(Pixmap<T>       &tgt,
                            const Pixmap<U> &src,
                            FUNC            &func,
                            Engine          &engine)
            {
                target = &tgt;
                source = &src;
                proc   = (void*)&func;
                engine.submit(this, & Mapper::callThread<T,U,FUNC> );
            }

            Mapper() throw();
            virtual ~Mapper() throw();

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Mapper);
            void       *target;
            const void *source;
            void       *proc;

            template <typename T,typename U,typename FUNC> inline
            void callThread( const Area &a, threading::context & ) throw()
            {
                YOCTO_INK_AREA_LIMITS(a);
                
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

