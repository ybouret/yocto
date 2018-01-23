#ifndef YOCTO_INK_BITMAP_INCLUDED
#define YOCTO_INK_BITMAP_INCLUDED 1

#include "yocto/ink/types.hpp"
#include "yocto/counted-object.hpp"

namespace yocto
{
    namespace Ink
    {


        //! memory handling
        class Bitmap : public counted_object, public Patch
        {
        public:
            enum MemoryType
            {
                MemoryIsGlobal, //!< standalone
                MemoryIsShared, //!< sub bitmap
                MemoryFromUser  //!< based on user's data
            };
            typedef void * (*xShiftProc)(void *,const unit_t dx);

            void            *addr;     //!< (0,0) address
            const unit_t     d;        //!< depth
            const unit_t     w;        //!< width>0
            const unit_t     h;        //!< height>0
            const xShiftProc xshift;   //!< fast addr shifting
            const unit_t     scanline; //!< w*d
            const unit_t     stride;   //!< stide>=scanline
            const MemoryType type;     //!< kind of Bitmap
            
            //! create a global bitmap
            explicit Bitmap(const unit_t W,
                            const unit_t H,
                            const unit_t D);
            virtual ~Bitmap() throw();

        private:
            YOCTO_DISABLE_ASSIGN(Bitmap);
            size_t allocated;
        };
    }
}

#endif

