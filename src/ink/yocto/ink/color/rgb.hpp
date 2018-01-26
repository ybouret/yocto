#ifndef YOCTO_INK_COLOR_RGB_INCLUDED
#define YOCTO_INK_COLOR_RGB_INCLUDED 1

#include "yocto/ink/color/pixel.hpp"

namespace yocto
{
    namespace Ink
    {
        template <typename T>
        class rgb
        {
        public:
            typedef Pixel<T> pixel_type;

            T r,g,b;
            inline  rgb() throw() : r(pixel_type::zero), g(pixel_type::zero), b(pixel_type::zero) {}
            inline ~rgb() throw() {}
            inline  rgb(const rgb &other) throw() : r(other.r), g(other.b), b(other.b) {}
            inline  rgb & operator=(const rgb &other) throw()
            {
                r=other.r;
                g=other.g;
                b=other.b;
                return *this;
            }
            inline rgb(T R, T G, T B) throw() : r(R), g(G), b(B) {}
        };

        template <typename T>
        class rgba
        {
        public:
            typedef Pixel<T> pixel_type;

            T r,g,b,a;
            inline  rgba() throw() : r(pixel_type::zero), g(pixel_type::zero), b(pixel_type::zero), a(pixel_type::opaque) {}
            inline ~rgba() throw() {}
            inline  rgba(const rgba &other) throw() : r(other.r), g(other.b), b(other.b), a(other.a) {}
            inline  rgba & operator=(const rgba &other) throw()
            {
                r=other.r;
                g=other.g;
                b=other.b;
                a=other.a;
                return *this;
            }
            inline rgba(T R, T G, T B, T A=pixel_type::opaque) throw() : r(R), g(G), b(B), a(A) {}
            inline rgba( const rgb<T> &other ) throw() : r(other.r), g(other.b), b(other.b), a(pixel_type::opaque) {}
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

