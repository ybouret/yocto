#ifndef YOCTO_INK_COLOR_YUV_INCLUDED
#define YOCTO_INK_COLOR_YUV_INCLUDED 1

#include "yocto/ink/color/rgb.hpp"
#include "yocto/code/utils.hpp"

namespace yocto
{
    namespace Ink
    {

        class YUV
        {
        public:
            typedef float type;
            float y,u,v;
            inline  YUV(const float Y=0,const float U=0,const float V=0) throw() : y(Y), u(U), v(V) {}
            inline ~YUV() throw() {}
            inline  YUV(const YUV &other) throw() : y(other.y), u(other.u), v(other.v) {}
            inline  YUV & operator=( const YUV &other ) throw()
            {
                y=other.y; u=other.u; v=other.v;
                return *this;
            }

            static inline
            RGB toRGB(const YUV &C) throw()
            {
                const float rf = C.y                  + 1.13983f*C.v;
                const float gf = C.y - 0.39465f * C.u - 0.58060f*C.v;
                const float bf = C.y + 2.03211f * C.u;
                return RGB(YOCTO_INK_F2B(clamp<float>(0,rf,1)),
                           YOCTO_INK_F2B(clamp<float>(0,gf,1)),
                           YOCTO_INK_F2B(clamp<float>(0,bf,1))
                           );
            }

            static inline
            YUV fromRGB(const RGB &C) throw()
            {
                const float rf = Core::uFloat[C.r];
                const float gf = Core::uFloat[C.g];
                const float bf = Core::uFloat[C.b];
                const float Y  =  0.29900f*rf+0.58700f*gf+0.11400f*bf;
                const float U  = -0.14713f*rf-0.28886f*gf+0.43600f*bf;
                const float V  =  0.61500f*rf-0.51499f*gf-0.10001f*bf;
                return YUV(Y,U,V);
            }

            static inline
            YUV fromRGBA(const RGBA &C) throw()
            {
                const RGB tmp(C.r,C.g,C.b);
                return fromRGB(tmp);
            }

        };
    }
}

#endif
