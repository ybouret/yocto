#ifndef YOCTO_INK_DRAW_CIRCLE_INCLUDED
#define YOCTO_INK_DRAW_CIRCLE_INCLUDED 1

#include "yocto/ink/draw/putpixel.hpp"

namespace yocto
{
    namespace Ink
    {
        namespace Draw
        {
            ////////////////////////////////////////////////////////////////////
            //
            // Circle(s)
            //
            ////////////////////////////////////////////////////////////////////
            
            //! draw a circle on an image
            template <typename T>
            inline void Circle(Pixmap<T>    &img,
                               const unit_t  xm,
                               const unit_t  ym,
                               unit_t        r,
                               YOCTO_INK_DRAW_ARGS) throw()
            {
                
                if(r<=0)
                {
                    assert(0==r);
                    const coord q(xm,ym);
                    if(img.contains(q))
                    {
                        proc(img,q,args);
                    }
                }
                else
                {
                    unit_t x = -r, y = 0, err = 2-2*r; /* bottom left to top right */
                    do {
                        const coord v[4] =
                        {
                            coord(xm-x,ym+y),  /*   I. Quadrant +x +y */
                            coord(xm-y,ym-x),  /*  II. Quadrant -x +y */
                            coord(xm+x,ym-y),  /* III. Quadrant -x -y */
                            coord(xm+y,ym+x)   /*  IV. Quadrant +x -y */
                        };
                        for(size_t i=0;i<4;++i)
                        {
                            const coord p = v[i];
                            if(img.contains(p))
                            {
                                proc( img, p, args );
                            }
                        }
                        r = err;
                        if (r <= y)
                        {
                            err += ++y*2+1;            /* e_xy+e_y < 0 */
                        }
                        if (r > x || err > y)
                        {/* e_xy+e_x > 0 or no 2nd y-step */
                            err += ++x*2+1;
                        }/* -> x-step now */
                    } while (x < 0);
                }
            }
            
            template <typename T>
            inline void Circle(Pixmap<T>    &img,
                               const unit_t  xm,
                               const unit_t  ym,
                               const unit_t  r,
                               const T       C)
            {
                Circle(img,xm,ym,r,PutPixel::Copy<T>,(void*)&C);
            }
            
            template <typename T>
            inline void Circle(Pixmap<T>    &img,
                               const unit_t  xm,
                               const unit_t  ym,
                               const unit_t  r,
                               const T       C,
                               const uint8_t alpha)
            {
                PutPixel::BlendInfo<T> blend(C,alpha);
                Circle(img,xm,ym,r,PutPixel::Blend<T>,(void*)&blend);
            }

            template <typename T>
            inline void Circle(Pixmap<T>    &img,
                               const unit_t  xm,
                               const unit_t  ym,
                               const unit_t  r,
                               Mask         &mask)
            {
                 Circle(img,xm,ym,r,PutPixel::Load<T>,&mask);
            }
            
            ////////////////////////////////////////////////////////////////////
            //
            // Disk(s)
            //
            ////////////////////////////////////////////////////////////////////
            template <typename T>
            inline void __HSeg(Pixmap<T>    &img,
                               const unit_t  y,
                               const unit_t  xlo,
                               const unit_t  xhi,
                               YOCTO_INK_DRAW_ARGS) throw()
            {
                assert(xlo<=xhi);
                coord p(xlo,y);
                for(;p.x<=xhi;++p.x)
                {
                    if(img.contains(p))
                    {
                        proc(img,p,args);
                    }
                }
            }
            
            template <typename T>
            inline void Disk(Pixmap<T>    &img,
                             const unit_t  xm,
                             const unit_t  ym,
                             unit_t        r,
                             YOCTO_INK_DRAW_ARGS) throw()
            {
                if(r<=0)
                {
                    const coord q(xm,ym);
                    if(img.contains(q))
                    {
                        proc(img,q,args);
                    }
                }
                else
                {
                    unit_t x = -r, y = 0, err = 2-2*r; /* bottom left to top right */
                    bool new_y = true;
                    do {
                        if(new_y)
                        {
                            __HSeg(img,ym-y,xm+x,xm-x,proc,args);
                            if(y>0)
                            {
                                __HSeg(img,ym+y,xm+x,xm-x,proc,args);
                            }
                            new_y = false;
                        }
                        r = err;
                        if (r <= y)
                        {
                            err += ++y*2+1;            /* e_xy+e_y < 0 */
                            new_y = true;
                        }
                        if (r > x || err > y)
                        {/* e_xy+e_x > 0 or no 2nd y-step */
                            err += ++x*2+1;                    /* -> x-step now */
                        }
                    } while (x < 0);
                }
            }
            
            template <typename T>
            inline void Disk(Pixmap<T>    &img,
                             const unit_t  xm,
                             const unit_t  ym,
                             const unit_t  r,
                             const T       C)
            {
                Disk(img,xm,ym,r,PutPixel::Copy<T>,(void*)&C);
            }
            
            template <typename T>
            inline void Disk(Pixmap<T>    &img,
                             const unit_t  xm,
                             const unit_t  ym,
                             const unit_t  r,
                             const T       C,
                             const uint8_t alpha)
            {
                PutPixel::BlendInfo<T> blend(C,alpha);
                Disk(img,xm,ym,r,PutPixel::Blend<T>,(void*)&blend);
            }

            template <typename T>
            inline void Disk(Pixmap<T>    &img,
                             const unit_t  xm,
                             const unit_t  ym,
                             const unit_t  r,
                             Mask         &mask)
            {
                Disk(img,xm,ym,r,PutPixel::Load<T>,&mask);
            }
        }
    }
}

#endif

