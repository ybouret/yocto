#ifndef YOCTO_INK_IMAGE_INCLUDED
#define YOCTO_INK_IMAGE_INCLUDED 1

#include "yocto/ink/bitmap.hpp"
#include "yocto/ink/color/rgba2data.hpp"
#include "yocto/ink/color/data2rgba.hpp"

namespace yocto
{
    namespace Ink
    {
        struct Image
        {
            class Format : public counted_object
            {
            public:
                const string name;
                virtual ~Format() throw();

            protected:
                explicit Format(const char *id);

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Format);
            };
        };

    }
}

#endif

