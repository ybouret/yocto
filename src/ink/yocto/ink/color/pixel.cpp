#include "yocto/ink/color/pixel.hpp"

namespace yocto
{

    namespace Ink
    {
        template <> float Pixel<float>::zero   = 0.0f;
        template <> float Pixel<float>::opaque = 1.0f;

        template <> double Pixel<double>::zero   = 0.0;
        template <> double Pixel<double>::opaque = 1.0;

        template <> uint8_t Pixel<uint8_t>::zero   = 0x00;
        template <> uint8_t Pixel<uint8_t>::opaque = 0xff;

    }

}


