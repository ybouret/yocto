
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
            
            //! scan circle
            template <typename T>
            inline bool scanCircle(float        &d2,
                                   Pixmap<T>    &img,
                                   const unit_t  xm,
                                   const unit_t  ym,
                                   unit_t        r) throw()
            {
                assert(r>0);
                bool has_data = false;
                unit_t x = -r, y = 0, err = 2-2*r; /* bottom left to top right */
                float  d2_min = (d2 = -1.0f);
                do {
                  
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
                                const unit_t x2 = x*x;
                                const unit_t y2 = y*y;
                                const float d2_tmp = x2+y2;
                                if(d2_min<0.0f)
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
                    {/* e_xy+e_x > 0 or no 2nd y-step */
                        err += ++x*2+1;
                    }/* -> x-step now */
                } while (x < 0);
                return has_data;
            }
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(EDM);
        };
        
    }
}

#endif

