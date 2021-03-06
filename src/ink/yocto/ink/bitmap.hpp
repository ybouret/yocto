#ifndef YOCTO_INK_BITMAP_INCLUDED
#define YOCTO_INK_BITMAP_INCLUDED 1

#include "yocto/ink/area.hpp"
#include "yocto/counted-object.hpp"
#include "yocto/string.hpp"
#include "yocto/exception.hpp"

namespace yocto
{
    namespace Ink
    {

        class Bitmap : public Area, public counted
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
            const unit_t  scanline;  //!< w*depth bytes for one line
            const unit_t  stride;    //!< stride>=pitch, bytes to change line
            const XShift  xshift;

            //! create an allocated bitmap
            explicit Bitmap(const unit_t D,
                            const unit_t W,
                            const unit_t H);

            //! create a Bitmap from user's data
            explicit Bitmap(void *user_data,
                            const unit_t D,
                            const unit_t W,
                            const unit_t H,
                            const unit_t Stride);

            //! create a shared bitmap
            explicit Bitmap( Bitmap *shared );

            //! copy with the same memory model
            Bitmap(const Bitmap &other);

            //! create a sub bitmap with the same memory model
            explicit Bitmap( const Bitmap &other, const Area &rect);

            virtual ~Bitmap() throw();


            void *       get(const unit_t x, const unit_t y) throw();
            const void * get(const unit_t x, const unit_t y) const throw();
            void       * get_line(const unit_t y) throw();
            const void * get_line(const unit_t y) const throw();
            void ldz() throw();
            void fill_with(const void *data) throw(); //!< assume depth bytes@data
            void copy(const Bitmap &other) throw();
            void flip_vertical()   throw();   //!< inverse rows
            void flip_horizontal() throw();   //!< inverse columns

            //! save as uncompressed bitmap
            /**
             assuming RGB or RGBA !!!
             - .bmp
             - .ppm
             - .eps
             */
            void save(const string &filename,
                      const bool    in_color = true ) const;

            unit_t zfx(unit_t x) const throw();
            unit_t zfy(unit_t y) const throw();

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

