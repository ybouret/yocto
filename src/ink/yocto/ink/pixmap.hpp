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

            class Row
            {
            public:
                inline Row(T *addr,const unit_t W) throw() : entry(addr), w(W)
                {
                    assert(entry); assert(w>0);
                }
                inline ~Row() throw();

                inline T & operator[](const unit_t x) throw()
                {
                    assert(x>=0); assert(x<w); return entry[x];
                }

                inline const T & operator[](const unit_t x) const throw()
                {
                    assert(x>=0); assert(x<w); return entry[x];
                }

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Row);
                T *entry;
            public:
                const unit_t w;
            };

            inline Row & operator[](const unit_t y) throw()
            {
                assert(y>=0); assert(y<h); return rows[y];
            }

            inline const Row & operator[](const unit_t y) const throw()
            {
                assert(y>=0); assert(y<h); return rows[y];
            }

            inline virtual ~Pixmap() throw()
            {
                deleteRows();
            }

#define YOCTO_INK_PIXMAP_PRV_CTOR() nrow(0), rows(0)
            inline Pixmap(const unit_t W,const unit_t H) :
            Bitmap(sizeof(T),W,H),
            YOCTO_INK_PIXMAP_PRV_CTOR()
            {
                createRows();
            }

            //! copy with same bitmap memory model
            inline Pixmap(const Pixmap &other) :
            Bitmap(other),
            YOCTO_INK_PIXMAP_PRV_CTOR()
            {
                createRows();
            }


        private:
            YOCTO_DISABLE_ASSIGN(Pixmap);
            size_t nrow;
            Row   *rows;

            inline void deleteRows() throw()
            {
                memory::kind<memory::global>::release_as<Row>(rows,nrow);
            }

            inline void createRows()
            {
                const size_t nr = size_t(h);
                nrow = nr;
                rows = memory::kind<memory::global>::acquire_as<Row>(nrow);
                uint8_t *p = static_cast<uint8_t *>(entry);
                for(size_t i=0;i<nr;++i,p+=stride)
                {
                    new (rows+i) Row((T*)p,w);
                }
            }
        };
    }
}

#endif

