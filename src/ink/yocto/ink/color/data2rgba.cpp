#include "yocto/ink/color/data2rgba.hpp"

namespace yocto
{
    namespace Ink
    {
        data2rgba::  data2rgba() throw() {}
        data2rgba:: ~data2rgba() throw() {}

        RGBA data2rgba:: operator()(const void *addr)
        {
            return get(addr);
        }

    }

}

namespace yocto
{
    namespace Ink
    {
        scale2rgba:: ~scale2rgba() throw() {}

        scale2rgba:: scale2rgba(const float vmin, const float vmax) throw() :
        smin( min_of(vmin,vmax) ),
        smax( max_of(vmin,vmax) ),
        delta( vmax-vmin )
        {
        }

        RGBA scale2rgba:: get(const void *addr)
        {
            const float   s = *(const float *)addr;
            const uint8_t u = (s<=smin) ? 0 : ( (s>smax) ? 255 : YOCTO_INK_F2B((s-smin)/delta) );
            return RGBA(u,u,u);
        }
    }

}

