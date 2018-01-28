#include "yocto/ink/color/pixel.hpp"

namespace yocto
{

    namespace Ink
    {
        template <> const float Pixel<float>::zero   = 0.0f;
        template <> const float Pixel<float>::opaque = 1.0f;

        template <> const double Pixel<double>::zero   = 0.0;
        template <> const double Pixel<double>::opaque = 1.0;

        template <> const uint8_t Pixel<uint8_t>::zero   = 0x00;
        template <> const uint8_t Pixel<uint8_t>::opaque = 0xff;

    }

}


