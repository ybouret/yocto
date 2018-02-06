#include "yocto/ink/color/ramp.hpp"

namespace yocto
{
    namespace Ink
    {
        ramp:: ~ramp() throw()
        {
        }

        ramp:: ramp(const float vmin, const float vmax, const RGBA *col256) throw() :
        vMin( min_of(vmin,vmax) ),
        vMax( max_of(vmin,vmax) ),
        factor( (vMin<vMax) ? (255.0f / (vMax-vMin)) : 0.0f ),
        colors(col256)
        {
            assert(col256);
            
        }

        RGBA ramp:: get(const void *addr)
        {
            const float v = *(const float *)addr;
            if(v<=vMin)
            {
                return colors[0];
            }
            else
            {
                if(v>=vMax)
                {
                    return colors[255];
                }
                else
                {
                    return colors[ uint8_t( floorf( factor * (v-vMin) + 0.5f ) ) ];
                }
            }

        }

    }
}
