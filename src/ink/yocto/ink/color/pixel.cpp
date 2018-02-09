#include "yocto/ink/color/pixel.hpp"

namespace yocto
{

    namespace Ink
    {
        template <> const float Pixel<float>::Zero   = 0.0f;
        template <> const float Pixel<float>::Opaque = 1.0f;
        template <> bool        Pixel<float>::IsZero(const float &x) throw() { return (x<=0.0f); }
        template <> float       Pixel<float>::Inverse(const float &x) throw() { return 1.0f-x; }

        template <> const double Pixel<double>::Zero   = 0.0;
        template <> const double Pixel<double>::Opaque = 1.0;
        template <> bool         Pixel<double>::IsZero(const double &x) throw() { return (x<=0.0); }
        template <> double       Pixel<double>::Inverse(const double &x) throw() { return 1.0-x; }


        template <> const uint8_t Pixel<uint8_t>::Zero   = 0x00;
        template <> const uint8_t Pixel<uint8_t>::Opaque = 0xff;
        template <> bool          Pixel<uint8_t>::IsZero(const uint8_t &x) throw() { return (x<=0); }
        template <> uint8_t       Pixel<uint8_t>::Inverse(const uint8_t &x) throw() { return 0xff-x; }

    }

}


