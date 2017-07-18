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

        template <typename T>
        inline void draw_stencil(const vector<vertex> &sten,
                                 const pixmap<T>      &img,
                                 const T               C,
                                 const uint8_t         alpha=0xff)
        {
            
        }
        
    }
}

#endif
