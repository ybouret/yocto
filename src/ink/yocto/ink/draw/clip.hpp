#ifndef YOCTO_INK_DRAW_CLIP_INCLUDED
#define YOCTO_INK_DRAW_CLIP_INCLUDED 1

#include "yocto/ink/area.hpp"

namespace yocto
{
    namespace Ink
    {

        struct Clip
        {
            static const size_t Inside = 0x00;
            static const size_t Left   = 0x01;
            static const size_t Right  = 0x02;
            static const size_t Bottom = 0x04;
            static const size_t Top    = 0x08;

            static
            size_t StatusOf( const unit_t x, const unit_t y, const Area &area ) throw();
            static inline
            size_t StatusOf( const coord c, const Area &area) throw() { return StatusOf(c.x,c.y,area); }
            static
            bool   Accept(unit_t &x0,
                          unit_t &y0,
                          unit_t &x1,
                          unit_t &y1,
                          const Area &area ) throw();
        };

    }
}

#endif

