#ifndef YOCTO_INK_BITMAP_INCLUDED
#define YOCTO_INK_BITMAP_INCLUDED 1

#include "yocto/ink/types.hpp"
#include "yocto/counted-object.hpp"

namespace yocto
{
    namespace Ink
    {

        class Bitmap : public counted_object
        {
        public:
            enum MemoryModel
            {
                MemoryIsGlobal,
                MemoryIsShared,
                MemoryFromUser
            };

            struct __Row { void *p; Coord w; };

            typedef void * (*XShift)(void *,const Coord);

            void        *entry; //!< address of (0,0)
            const Coord  depth;
            const Coord  w;
            const Coord  h;
            const Coord  pitch;  //!< w*depth bytes for one line
            const Coord  stride; //!< stride>=picth, bytes to change line
            const XShift xshift;

            explicit Bitmap();
            virtual ~Bitmap() throw();

        protected:
            void *__rows; //!< memory for rows

        private:
            YOCTO_DISABLE_ASSIGN(Bitmap);
        };
        
    }
}

#endif

