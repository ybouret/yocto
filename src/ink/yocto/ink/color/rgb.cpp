
#include "yocto/ink/color/rgb.hpp"

namespace yocto
{
    namespace Ink
    {
        template <>
        bool Pixel<RGB>::IsZero(const RGB &C) throw() { return (C.r<=0) && (C.g<=0) && (C.b<=0); }

        template <>
        RGB Pixel<RGB>::Inverse(const RGB &C) throw() { return RGB(0xff-C.r,0xff-C.g,0xff-C.b);  }
    }
}

