#ifndef YOCTO_GFX_DRAW_STENCIL
#define YOCTO_GFX_DRAW_STENCIL 1

#include "yocto/gfx/draw/line.hpp"
#include "yocto/gfx/draw/circle.hpp"
#include "yocto/sort/unique.hpp"
#include <cstring>

namespace yocto
{
    namespace gfx
    {
        inline int  compare_vertices(const vertex &lhs, const vertex &rhs) throw()
        {
            return memcmp(&lhs, &rhs, sizeof(vertex) );
        }
        
        inline void clean_stencil(vector<vertex> &sten)
        {
            unique(sten,compare_vertices);
        }
        
        inline void extrude_stencil(vector<vertex> &sten, const vertex shift)
        {
            for(size_t i=sten.size();i>0;--i)
            {
                const vertex p = sten[i];
                const vertex q = p + shift;
                collect_line(sten, p.x, p.y, q.x, q.y);
            }
            clean_stencil(sten);
        }

        template <typename T>
        inline void draw_stencil(const vector<vertex> &sten,
                                 const pixmap<T>      &img,
                                 const vertex          vec,
                                 const T               C,
                                 const uint8_t         alpha=0xff)
        {
            for(size_t i=sten.size();i>0;--i)
            {
                //const vertex ini = sten[i];
                const vertex p = sten[i] + vec;
                if(img.has(p))
                {
                    img[p] = pixel<T>::blend(img[p],C,alpha);
                }
                
            }
        }
        
    }
}

#endif
