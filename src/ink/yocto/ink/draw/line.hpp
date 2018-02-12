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

#define YOCTO_INK_DRAW_ARGS void  (*proc)( T &bg, void *args), void   *args

#define YOCTO_INK_PUTPIXEL(I) proc( p[I], args)

            template <typename T>
            inline void _HLine(Pixmap<T>    &pxm,
                               const unit_t  x,
                               const unit_t  y,
                               const size_t  w,
                               YOCTO_INK_DRAW_ARGS)
            {
                assert(proc);
                assert( pxm.has(x,y)     );
                assert( pxm.has(x+w-1,y) );
                assert(w>0);

                typename Pixmap<T>::Row &r = pxm[y];
                T                       *p = &r[x];
                YOCTO_LOOP_FUNC_(w,YOCTO_INK_PUTPIXEL,0);
            }

            template <typename T>
            inline void HLine(Pixmap<T>    &pxm,
                              unit_t        x,
                              const unit_t  y,
                              size_t        w,
                              YOCTO_INK_DRAW_ARGS)
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
            inline void HLine(Pixmap<T>   &pxm,
                              const unit_t x,
                              const unit_t y,
                              const size_t w,
                              const T      C)
            {
                HLine(pxm,x,y,w,PutPixel::Copy<T>,(void*)&C);
            }

            template <typename T>
            inline void HLine(Pixmap<T>   & pxm,
                              const unit_t  x,
                              const unit_t  y,
                              const size_t  w,
                              const T       C,
                              const uint8_t alpha)
            {
                PutPixel::BlendInfo<T> blend(C,alpha);
                HLine(pxm,x,y,w,PutPixel::Blend<T>,(void*)&blend);
            }
        }
    }
}

namespace yocto
{
    namespace Ink
    {
        namespace Draw
        {
            template <typename T>
            inline void _VLine(Pixmap<T>    &pxm,
                               const unit_t  x,
                               const unit_t  y,
                               const size_t  h,
                               YOCTO_INK_DRAW_ARGS)
            {
                assert(proc);
                assert( pxm.has(x,y)     );
                assert( pxm.has(x,y+h-1) );
                assert(h>0);

                for(size_t j=0;j<h;++j)
                {
                    proc(pxm[j+y][x],args);
                }
            }

            template <typename T>
            inline void VLine(Pixmap<T>    &pxm,
                              const unit_t  x,
                              unit_t        y,
                              size_t        h,
                              YOCTO_INK_DRAW_ARGS)
            {
                //check
                unit_t y_end=y+h-1;
                if(h<=0||x<pxm.x||x>pxm.x_end||y>pxm.y_end||y_end<pxm.x) return;

                //clip
                if(y<pxm.y)         y    =pxm.y;
                if(y_end>pxm.y_end) y_end=pxm.y_end;
                h = (y_end-y) + 1; assert(h>0);

                //draw
                _VLine(pxm, x, y, h, proc, args);
            }

            template <typename T>
            inline void VLine(Pixmap<T>    &pxm,
                              const unit_t  x,
                              const unit_t  y,
                              const size_t  h,
                              const T       C)
            {
                VLine(pxm,x,y,h,PutPixel::Copy<T>,(void*)&C);
            }

            template <typename T>
            inline void VLine(Pixmap<T>    &pxm,
                              const unit_t  x,
                              const unit_t  y,
                              const size_t  h,
                              const T       C,
                              const unit_t  alpha)
            {
                PutPixel::BlendInfo<T> blend(C,alpha);
                VLine(pxm,x,y,h,PutPixel::Blend<T>,(void*)&blend);
            }

        }

    }
}


namespace yocto
{
    namespace Ink
    {
        namespace Draw
        {
            //! draw a line on an image
            template <typename T>
            inline void _Line(Pixmap<T>    &img,
                              unit_t        x0,
                              unit_t        y0,
                              unit_t        x1,
                              unit_t        y1,
                              YOCTO_INK_DRAW_ARGS)
            {
                unit_t dx =  abs_of(x1-x0), sx = (x0<x1) ? 1 : -1;
                unit_t dy = -abs_of(y1-y0), sy = (y0<y1) ? 1 : -1;
                unit_t err = dx+dy, e2;                                   /* error value e_xy */
                assert(img.has(x0,y0));
                assert(img.has(x1,y1));
                for (;;){
                    /* loop */
                    const coord p(x0,y0);
                    proc( img[p], args);
                    e2 = 2*err;
                    if (e2 >= dy) {                                         /* e_xy+e_x > 0 */
                        if (x0 == x1) break;
                        err += dy; x0 += sx;
                    }
                    if (e2 <= dx) {                                         /* e_xy+e_y < 0 */
                        if (y0 == y1) break;
                        err += dx; y0 += sy;
                    }
                }
            }

            template <typename T>
            inline void Line(Pixmap<T>    &img,
                             unit_t        x0,
                             unit_t        y0,
                             unit_t        x1,
                             unit_t        y1,
                             YOCTO_INK_DRAW_ARGS)
            {
                if( Clip::Accept(x0,y0,x1,y1,img) )
                {
                    _Line(img,x0,y0,x1,y1,proc,args);
                }
            }

            template <typename T>
            inline void Line(Pixmap<T>    &img,
                             const unit_t  x0,
                             const unit_t  y0,
                             const unit_t  x1,
                             const unit_t  y1,
                             const T       C)
            {
                Line(img,x0,y0,x1,y1,PutPixel::Copy<T>,(void*)&C);
            }

            template <typename T>
            inline void Line(Pixmap<T>    &img,
                             const unit_t  x0,
                             const unit_t  y0,
                             const unit_t  x1,
                             const unit_t  y1,
                             const T       C,
                             const uint8_t alpha)
            {
                PutPixel::BlendInfo<T> blend(C,alpha);
                Line(img,x0,y0,x1,y1,PutPixel::Blend<T>,(void*)&blend);
            }

        }
    }
}


namespace yocto
{
    namespace Ink
    {
        namespace Draw
        {
            template <typename T>
            void Fill(Pixmap<T> &pxm,
                      unit_t x0,
                      unit_t y0,
                      unit_t x1,
                      unit_t y1,
                      YOCTO_INK_DRAW_ARGS)
            {
                //std::cerr << "Fill (" << x0 << "," << y0 << ") => ("<< x0 << "," << y0 << ")" << std::endl;
                if(x0>x1) cswap(x0,x1);
                if(y0>y1) cswap(y0,y1);
                if( !Clip::Accept(x0, y0, x1, y1, pxm) )
                    return;
                assert(x0<=x1);
                assert(y0<=y1);
                for(unit_t j=y1;j>=y0;--j)
                {
                    typename Pixmap<T>::Row &r = pxm[j];
                    for(unit_t i=x1;i>=x0;--i)
                    {
                        proc(r[i],args);
                    }
                }
            }

            template <typename T>
            inline void Fill(Pixmap<T>    &img,
                             const unit_t  x0,
                             const unit_t  y0,
                             const unit_t  x1,
                             const unit_t  y1,
                             const T       C)
            {
                Fill(img,x0,y0,x1,y1,PutPixel::Copy<T>,(void*)&C);
            }

            template <typename T>
            inline void Fill(Pixmap<T>    &img,
                             const unit_t  x0,
                             const unit_t  y0,
                             const unit_t  x1,
                             const unit_t  y1,
                             const T       C,
                             const uint8_t alpha)
            {
                PutPixel::BlendInfo<T> blend(C,alpha);
                Fill(img,x0,y0,x1,y1,PutPixel::Blend<T>,(void*)&blend);
            }
        }
    }
}

#endif

