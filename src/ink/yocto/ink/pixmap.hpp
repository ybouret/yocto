#ifndef YOCTO_INK_PIXMAP_INCLUDED
#define YOCTO_INK_PIXMAP_INCLUDED 1

#include "yocto/ink/types.hpp"

namespace yocto
{
    namespace Ink
    {

        template <typename T>
        class Pixmap : public Patch
        {
        public:
            

            inline virtual ~Pixmap() throw()
            {
            }



        private:
            YOCTO_DISABLE_ASSIGN(Pixmap);
        };
    }
}

#endif

