
#include "yocto/ink/color/rgb.hpp"

namespace yocto
{
    namespace Ink
    {
        template <>
        const RGB Pixel<RGB>::Opaque(0xff,0xff,0xff);
        
        template <>
        const RGB Pixel<RGB>::Zero(0x00,0x00,0x00);

        template <>
        bool Pixel<RGB>::IsZero(const RGB &C) throw() { return (C.r<=0) && (C.g<=0) && (C.b<=0); }

        template <>
        RGB Pixel<RGB>::Inverse(const RGB &C) throw() { return RGB(0xff-C.r,0xff-C.g,0xff-C.b);  }

        template <> RGB Pixel<RGB>::Average(const RGB *value, const size_t count) throw()
        {
            assert(count>0);
            assert(value);
            unsigned R=0,G=0,B=0;
            for(size_t i=0;i<count;++i)
            {
                const RGB C = value[i];
                R += unsigned(C.r);
                G += unsigned(C.g);
                B += unsigned(C.b);
            }
            return RGB(uint8_t(R/count),uint8_t(G/count),uint8_t(B/count));
        }

#define _FG(X)  (fgw*unsigned((fg).X))
#define _BG(X)  (bgw*unsigned((bg).X))
#define _MIX(X) ( (_FG(X)+_BG(X))>>8 )

        template <> RGB Pixel<RGB>:: Blend(const RGB &fg, const RGB &bg, const uint8_t alpha) throw()
        {
            switch(alpha)
            {
                case 0xff: return fg;
                case 0x00: return bg;
                default:
                    break;
            }

            const unsigned fgw = alpha;
            const unsigned bgw = 0xff-alpha;
            return RGB( _MIX(r), _MIX(g), _MIX(b) );
        }

    }
}

