#ifndef YOCTO_GFX_DRAW_LINE_INCLUDED
#define YOCTO_GFX_DRAW_LINE_INCLUDED 1

#include "yocto/gfx/pixmap.hpp"
#include "yocto/gfx/pixel.hpp"

namespace yocto
{
    namespace gfx
    {
        //! draw a line on an image
        template <typename T>
        inline void draw_line(pixmap<T>    &img,
                              unit_t        x0,
                              unit_t        y0,
                              unit_t        x1,
                              unit_t        y1,
                              const  T      C,
                              const uint8_t alpha=0xff)
        {
            unit_t dx =  abs_of(x1-x0), sx = (x0<x1) ? 1 : -1;
            unit_t dy = -abs_of(y1-y0), sy = (y0<y1) ? 1 : -1;
            unit_t err = dx+dy, e2;                                   /* error value e_xy */

            for (;;){                                                          /* loop */
                //setPixel(x0,y0);
                const vertex p(x0,y0);
                if(img.has(p))
                {
                    img[p] = pixel<T>::blend(img[p],C,alpha);
                }
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

        //! collect all the single points of a line into a stencil
        inline void collect_line(sequence<vertex> &stencil,
                                 unit_t            x0,
                                 unit_t            y0,
                                 unit_t            x1,
                                 unit_t            y1)
        {
            unit_t dx =  abs_of(x1-x0), sx = (x0<x1) ? 1 : -1;
            unit_t dy = -abs_of(y1-y0), sy = (y0<y1) ? 1 : -1;
            unit_t err = dx+dy, e2;                                   /* error value e_xy */
            
            for (;;){                                                          /* loop */
                //setPixel(x0,y0);
                const vertex p(x0,y0);
                stencil.push_back(p);
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
        

        //! draw a line, wrapper
        template <typename T>
        inline void draw_line(pixmap<T>    &img,
                              vertex        v0,
                              vertex        v1,
                              const  T      C,
                              const uint8_t alpha=0xff)

        {
            draw_line<T>(img,v0.x,v0.y,v1.x,v1.y,C,alpha);
        }

        template <typename T>
        inline void draw_patch(pixmap<T>    &img,
                               const patch  &p,
                               const T       C,
                               const uint8_t alpha = 0xff )
        {
            draw_line<T>(img,p.lower.x,p.lower.y,p.lower.x,p.upper.y,C,alpha);
            draw_line<T>(img,p.lower.x,p.upper.y,p.upper.x,p.upper.y,C,alpha);
            draw_line<T>(img,p.upper.x,p.upper.y,p.upper.x,p.lower.y,C,alpha);
            draw_line<T>(img,p.upper.x,p.lower.y,p.lower.x,p.lower.y,C,alpha);
        }


    }
}

#endif
