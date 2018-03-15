
#ifndef YOCTO_INK_EDM_INCLUDED
#define YOCTO_INK_EDM_INCLUDED 1

#include "yocto/ink/pixmaps.hpp"

namespace yocto
{
    namespace Ink
    {
        
        class EDM
        {
        public:
            
            
            
            template <typename T>
            inline unit_t find_d2(Pixmap<T>    &img,
                                  const unit_t  xm,
                                  const unit_t  ym)
            {
                assert(img.contains(xm,ym));
                assert(!Pixel<T>::IsZero(img[ym][xm]));
                unit_t d2 = -1;
                for(unit_t r=1;;++r)
                {
                    if(!scanCircle(d2,img,xm, ym, r))
                    {
                        assert(d2<0);
                        return d2;
                    }
                    if(d2>=0)
                    {
                        return d2;
                    }
                }
            }
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(EDM);
            //! scan circle
            template <typename T>
            inline bool scanCircle(unit_t       &d2,
                                   Pixmap<T>    &img,
                                   const unit_t  xm,
                                   const unit_t  ym,
                                   unit_t        r) throw()
            {
                assert(r>0);
                bool has_data = false;
                unit_t x = -r, y = 0, err = 2-2*r; /* bottom left to top right */
                unit_t d2_min = (d2 = -1);
                do {
                    // there are four possible points at the same distance
                    const coord v[4] =
                    {
                        coord(xm-x,ym+y),  /*   I. Quadrant +x +y */
                        coord(xm-y,ym-x),  /*  II. Quadrant -x +y */
                        coord(xm+x,ym-y),  /* III. Quadrant -x -y */
                        coord(xm+y,ym+x)   /*  IV. Quadrant +x -y */
                    };
                    
                    // testing the four points, at the same distance
                    for(size_t i=0;i<4;++i)
                    {
                        const coord p = v[i];
                        if(img.contains(p))
                        {
                            has_data = true;
                            if( Pixel<T>::IsZero(img[p]))
                            {
                                const unit_t x2     = x*x;
                                const unit_t y2     = y*y;
                                const float  d2_tmp = x2+y2;
                                if(d2_min<0)
                                {
                                    d2_min = d2_tmp;
                                }
                                else
                                {
                                    if(d2_tmp<d2_min)
                                    {
                                        d2_min = d2_tmp;
                                    }
                                }
                            }
                        }
                    }
                    
                    r = err;
                    if (r <= y)
                    {
                        err += ++y*2+1;            /* e_xy+e_y < 0 */
                    }
                    if (r > x || err > y)
                    {
                        err += ++x*2+1;
                    }
                } while (x < 0);
                d2 = d2_min;
                return has_data;
            }
        };
        
    }
}

#endif

