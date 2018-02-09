#ifndef YOCTO_INK_COLOR_RGB_INCLUDED
#define YOCTO_INK_COLOR_RGB_INCLUDED 1

#include "yocto/ink/color/pixel.hpp"

#if defined(RGB)
#undef RGB
#endif

#if defined(RGBA)
#undef RGBA
#endif

YOCTO_INK_PIXEL_EXTERN();

namespace yocto
{
    namespace Ink
    {
        template <typename T>
        class rgb
        {
        public:
            typedef Pixel<T> PixelType;

            T r,g,b;
            inline  rgb() throw() : r(PixelType::Zero), g(PixelType::Zero), b(PixelType::Zero) {}
            inline ~rgb() throw() {}
            inline  rgb(const rgb &other) throw() : r(other.r), g(other.g), b(other.b) {}
            inline  rgb & operator=(const rgb &other) throw()
            {
                r=other.r;
                g=other.g;
                b=other.b;
                return *this;
            }
            inline rgb(T R, T G, T B) throw() : r(R), g(G), b(B) {}

            inline float intensity() const throw() { return Core::GreyScaleF(r,g,b); }
            inline friend bool operator<( const rgb &lhs, const rgb &rhs) throw()
            {
                return lhs.intensity() < rhs.intensity();
            }

        };

        template <typename T>
        class rgba
        {
        public:
            typedef Pixel<T> PixelType;

            T r,g,b,a;
            inline  rgba() throw() : r(PixelType::Zero), g(PixelType::Zero), b(PixelType::Zero), a(PixelType::Opaque) {}
            inline ~rgba() throw() {}
            inline  rgba(const rgba &other) throw() : r(other.r), g(other.g), b(other.b), a(other.a) {}
            inline  rgba & operator=(const rgba &other) throw()
            {
                r=other.r;
                g=other.g;
                b=other.b;
                a=other.a;
                return *this;
            }
            
            inline rgba(T R, T G, T B, T A=PixelType::Opaque) throw() : r(R), g(G), b(B), a(A) {}
            inline rgba( const rgb<T> &other ) throw() : r(other.r), g(other.g), b(other.b), a(PixelType::Opaque) {}
            inline rgba & operator=(const rgb<T> &other) throw()
            {
                r=other.r;
                g=other.g;
                b=other.b;
                return *this;
            }
        };


        typedef rgb<uint8_t>  RGB;
        typedef rgba<uint8_t> RGBA;

    }
}



#endif

