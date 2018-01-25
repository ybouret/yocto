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

            //! shadow Row layout
            struct __Row { void *p; unit_t w; };

            typedef void * (*XShift)(void *,const unit_t);

            void         *entry; //!< address of (0,0)
            const unit_t  depth;
            const unit_t  w;
            const unit_t  h;
            const unit_t  pitch;  //!< w*depth bytes for one line
            const unit_t  stride; //!< stride>=pitch, bytes to change line
            const XShift  xshift;

            //! create an allocated bitmap
            explicit Bitmap(const unit_t D,
                            const unit_t W,
                            const unit_t H);

            

            //! create a shared bitmap
            explicit Bitmap( Bitmap *shared );

            //! copy with the same memory model
            Bitmap(const Bitmap &other);

            virtual ~Bitmap() throw();

            void ldz() throw();
            void copy(const Bitmap &other) throw();
            void flip_vertical() throw();   //!< inverse rows
            void flip_horizontal() throw(); //!< inverse columns

        protected:
            void *_rows; //!< memory for rows

        private:
            YOCTO_DISABLE_ASSIGN(Bitmap);
            Bitmap *shBitmap;
            void   *prv_data;
            size_t  prv_size;

            void    allocate();
            void    allocate_rows_only();
            void    link_rows() throw();
            
        public:
            const MemoryModel model;
        };
        
    }
}

#endif

