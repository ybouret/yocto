#ifndef YOCTO_INK_COLOR_DATA2RGBA_INCLUDED
#define YOCTO_INK_COLOR_DATA2RGBA_INCLUDED 1

#include "yocto/ink/color/yuv.hpp"

namespace yocto
{
    namespace Ink
    {

        //! create RGBA from data
        class data2rgba : public object
        {
        public:
            virtual ~data2rgba() throw();

            RGBA operator()(const void *addr);
            
        protected:
            explicit data2rgba() throw();

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(data2rgba);
            virtual RGBA get(const void *addr) = 0;
        };


#define YOCTO_INK_DATA2RGBA(CLASS,CODE)          \
class CLASS : public data2rgba {                     \
public:                                              \
inline explicit CLASS() throw() : data2rgba() {}     \
inline virtual ~CLASS() throw() {}                   \
private:                                             \
YOCTO_DISABLE_COPY_AND_ASSIGN(CLASS);                \
virtual RGBA get(const void *addr) { CODE; } \
}

        //! for pixmap4
        YOCTO_INK_DATA2RGBA(rgba2rgba,return *(const RGBA*)addr);

        //! for pixmap3
        YOCTO_INK_DATA2RGBA(rgb2rgba, return RGBA(*(const RGB*)addr) );

        //! for pixmapf
        YOCTO_INK_DATA2RGBA(float2rgba,
                                const uint8_t u=YOCTO_INK_F2B(*(const float *)addr);
                                return RGBA(u,u,u)
                                );


        //! for pixmap1
        YOCTO_INK_DATA2RGBA(byte2rgba,
                            const uint8_t u=*(const uint8_t *)addr;
                            return RGBA(u,u,u)
                            );


        //! for pixmap 1,red-scale
        YOCTO_INK_DATA2RGBA(red2rgba,   return RGBA(*(const uint8_t*)addr,0,0,255) );

        //! for pixmap 1, green-scale
        YOCTO_INK_DATA2RGBA(green2rgba, return RGBA(0,*(const uint8_t*)addr,0,255) );

        //! for pixmap 1, blue-scale
        YOCTO_INK_DATA2RGBA(blue2rgba,  return RGBA(0,0,*(const uint8_t*)addr,255) );

        //! for complex to greyscale
        YOCTO_INK_DATA2RGBA(cplx2rgba,
                                const cplx_t   c = *(const cplx_t *)addr;
                                const uint8_t  u = YOCTO_INK_F2B(c.mod());
                                return RGBA(u,u,u)
                                );

        //! for YUV
        YOCTO_INK_DATA2RGBA(yuv2rgba,return YUV::toRGB(*(const YUV*)addr) );

        //! for scaling
        class scale2rgba : public data2rgba
        {
        public:
            explicit scale2rgba(const float vmin, const float vmax) throw();
            virtual ~scale2rgba() throw();
            const float smin;
            const float smax;
            const float delta;
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(scale2rgba);
            virtual RGBA get(const void *addr);
        };

        class yuv_u2rgba : public scale2rgba
        {
        public:
            inline explicit yuv_u2rgba() throw() : scale2rgba(-0.436f,0.436f) {}
            inline virtual ~yuv_u2rgba() throw() {}
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(yuv_u2rgba);
        };

        class yuv_v2rgba : public scale2rgba
        {
        public:
            inline explicit yuv_v2rgba() throw() : scale2rgba(-0.615f,0.615f) {}
            inline virtual ~yuv_v2rgba() throw() {}
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(yuv_v2rgba);
        };
    }

}

#endif
