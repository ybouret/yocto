#ifndef YOCTO_INK_COLOR_RAMP_INCLUDED
#define YOCTO_INK_COLOR_RAMP_INCLUDED 1

#include "yocto/ink/color/data2rgba.hpp"


namespace yocto
{
    namespace Ink
    {

        //! to convert a Pixmap<float>
        class ramp : public data2rgba
        {
        public:
            virtual ~ramp() throw();
            const float vMin;
            const float vMax;
            const float factor; //! 255.0f/dV;

            RGB  toRGB(const float f) const throw();
            RGBA toRGBA(const float f) const throw();

        protected:
            explicit ramp(const float vmin, const float vmax, const RGBA *col256) throw();

        private:
            const RGBA *colors;
            YOCTO_DISABLE_COPY_AND_ASSIGN(ramp);
            virtual RGBA get(const void *addr);
        };
    }
}

#endif
