#ifndef YOCTO_INK_PIXMAP_INCLUDED
#define YOCTO_INK_PIXMAP_INCLUDED 1

#include "yocto/ink/bitmap.hpp"

namespace yocto
{
    namespace Ink
    {
        template <typename T>
        class Pixmap : public Bitmap
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;

            //! shadow Row class
            class Row
            {
            private:
                type *entry;
            public:
                const unit_t w;

                inline type & operator[](const unit_t i) throw()
                { assert(entry); assert(i>=0);assert(i<w); return entry[i]; }

                inline const_type & operator[](const unit_t i) const throw()
                { assert(entry); assert(i>=0);assert(i<w); return entry[i]; }
                
            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Row);
                Row(); ~Row();
            };

            inline Row & operator[](const unit_t j) throw()
            {
                assert(_rows); assert(j>=0); assert(j<h); return *(static_cast<Row *>(_rows)+j);
            }

            inline const Row & operator[](const unit_t j) const throw()
            {
                assert(_rows); assert(j>=0); assert(j<h); return *(static_cast<Row *>(_rows)+j);
            }

            inline virtual ~Pixmap() throw() {}

            inline explicit Pixmap(const unit_t W,
                                   const unit_t H) :
            Bitmap(sizeof(T),W,H)
            {
            }
            
            inline Pixmap(const Pixmap &pxm) :
            Bitmap(pxm)
            {
            }
            

        private:
            YOCTO_DISABLE_ASSIGN(Pixmap);
        };
    }
}

#endif

