
#ifndef YOCTO_INK_COLOR_NAMED_INCLUDED
#define YOCTO_INK_COLOR_NAMED_INCLUDED 1

#include "yocto/ink/color/data2rgba.hpp"

namespace yocto
{
    namespace Ink
    {
        struct NamedColor
        {
            const char             *name;
            const uint8_t           r;
            const uint8_t           g;
            const uint8_t           b;
            static const NamedColor DB[];
            static const size_t     DB_SIZE;
            static RGB FetchRGB(const size_t indx) throw();
        };
        
        template <typename T>
        class indx2rgba : public data2rgba
        {
        public:
            size_t shift;
            inline explicit indx2rgba(const size_t sh) throw() : shift(sh) {}
            inline virtual ~indx2rgba() throw() {}
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(indx2rgba);
            inline virtual RGBA get(const void *addr)
            {
                const size_t indx = *(const T*)addr;
                if(indx<=0)
                {
                    return RGBA(0,0,0);
                }
                else
                {
                    return NamedColor::FetchRGB(indx+shift);
                }
            }
        };

    }
}

#endif
