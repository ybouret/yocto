#ifndef YOCTO_INK_DRAW_LINE_INCLUDED
#define YOCTO_INK_DRAW_LINE_INCKLUDED 1

#include "yocto/ink/draw/clip.hpp"
#include "yocto/ink/draw/putpixel.hpp"
#include "yocto/ink/pixmap.hpp"
#include "yocto/code/unroll.hpp"

namespace yocto
{
    namespace Ink
    {
        namespace Draw
        {
#define YOCTO_INK_PUTPIXEL(I) proc( p[I], args)

            template <typename T>
            inline void _HLine(Pixmap<T>    &pxm,
                               const unit_t  x,
                               const unit_t  y,
                               const size_t  w,
                               void  (*proc)( T &bg, void *args),
                               void   *args)
            {
                assert(proc);
                assert( pxm.has(x,y)     );
                assert( pxm.has(x+w-1,y) );

                typename Pixmap<T>::Row &r = pxm[y];
                T                       *p = &r[x];
                YOCTO_LOOP_FUNC(w,YOCTO_INK_PUTPIXEL,0);
            }

            template <typename T>
            inline void HLine(Pixmap<T>    &pxm,
                              unit_t        x,
                              unit_t        y,
                              size_t        w,
                              void  (*proc)( T &bg, void *args),
                              void   *args)
            {
                // check
                unit_t x_end = (x+w)-1;
                if(w<=0||(y<pxm.y)||(y>pxm.y_end)||(x>pxm.x_end)||(x_end<pxm.x)) return;

                // clip
                if(x<pxm.x)         x    =pxm.x;
                if(x_end>pxm.x_end) x_end=pxm.x_end;
                w = (x_end-x) + 1; assert(w>0);

                _HLine(pxm,x,y,w,proc,args);
            }

            template <typename T>
            inline void HLine(Pixmap<T>    &pxm,
                              unit_t        x,
                              unit_t        y,
                              size_t        w,
                              const T       C)
            {
                HLine(pxm,x,y,w,PutPixel::Copy<T>,(void*)&C);
            }

            template <typename T>
            inline void HLine(Pixmap<T>    &pxm,
                              unit_t        x,
                              unit_t        y,
                              size_t        w,
                              const T       C,
                              const uint8_t alpha)
            {
                PutPixel::BlendInfo<T> blend(C,alpha);
                HLine(pxm,x,y,w,PutPixel::Blend<T>,(void*)&blend);
            }
        }
    }
}

#endif

