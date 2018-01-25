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

            struct __Row { void *p; unit_t w; };

            typedef void * (*XShift)(void *,const unit_t);

            void         *entry; //!< address of (0,0)
            const unit_t  depth;
            const unit_t  w;
            const unit_t  h;
            const unit_t  pitch;  //!< w*depth bytes for one line
            const unit_t  stride; //!< stride>=picth, bytes to change line
            const XShift  xshift;

            explicit Bitmap(const unit_t D,
                            const unit_t W,
                            const unit_t H);
            
            virtual ~Bitmap() throw();

        protected:
            void *_rows; //!< memory for rows

        private:
            YOCTO_DISABLE_ASSIGN(Bitmap);
            void  *prv_data;
            size_t prv_size;
            void   link_rows() throw();
            
        public:
            const MemoryModel model;
        };
        
    }
}

#endif

