#ifndef YOCTO_INK_COLOR_RGBA2DATA_INCLUDED
#define YOCTO_INK_COLOR_RGBA2DATA_INCLUDED 1

#include "yocto/ink/color/rgb.hpp"
#include "yocto/ink/color/yuv.hpp"

namespace yocto
{
    namespace Ink
    {

        class rgba2data : public object
        {
        public:
            virtual ~rgba2data() throw();

            void operator()(void *addr, const RGBA &C);

        protected:
            explicit rgba2data() throw();

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(rgba2data);
            virtual void put(void *addr,const RGBA &C) = 0;
        };

#define YOCTO_INK_RGBA2DATA(CLASS,CODE)              \
class CLASS : public rgba2data {                     \
public:                                              \
inline explicit CLASS() throw() : rgba2data() {}     \
inline virtual ~CLASS() throw() {}                   \
private:                                             \
YOCTO_DISABLE_COPY_AND_ASSIGN(CLASS);                \
virtual void put(void *addr,const RGBA &C) { CODE; } \
}


        //! for pixmap4
        YOCTO_INK_RGBA2DATA(PutRGBA,*(RGBA *)addr=C);

        //! for pixmap3
        YOCTO_INK_RGBA2DATA(PutRGB,new (addr) RGB(C.r,C.g,C.b));

        //! for pixmapf, greyscale
        YOCTO_INK_RGBA2DATA(PutGSF,*(float *)addr =  Core::GreyScaleF(C.r, C.g, C.b) );

        //! for pixmap1, greyscale
        YOCTO_INK_RGBA2DATA(PutGSU,*(uint8_t*)addr = Core::GreyScale1(C.r, C.g, C.b) );

        //! for YUV
        YOCTO_INK_RGBA2DATA(PutYUV, new (addr) YUV( YUV::fromRGBA(C) ) );

    }

}
#endif

