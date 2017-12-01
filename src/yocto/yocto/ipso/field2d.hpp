#ifndef YOCTO_IPSO_FIELD2D_INCLUDED
#define YOCTO_IPSO_FIELD2D_INCLUDED 1

#include "yocto/ipso/field1d.hpp"

namespace yocto
{
    namespace ipso
    {
        template <typename T>
        class field2D : public patch2D
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            typedef field1D<T> row;

            inline explicit field2D( const patch2D &p, void *usr=NULL ) :
            patch2D(p),
            row_patch(p.lower.x,p.upper.x),
            entry(0),
            rows(0),
            wksp(0),
            wlen(0)
            {
                assert(this->width.x==row_patch.width);
                const size_t data_offset = 0;
                const size_t data_length = this->items * sizeof(type);
                const size_t rows_offset = memory::align(data_offset+data_length);
                const size_t rows_length = this->width.y * sizeof(row);
                if(usr)
                {
                    link(usr,data_offset,rows_offset);
                }
                else
                {
                    wlen = memory::align(rows_offset+rows_length);
                    wksp = memory::kind<memory::global>::acquire(wlen);
                    link(wksp,data_offset,rows_offset);
                }
            }


            inline virtual ~field2D() throw()
            {
                entry = 0;
                rows  = 0;
                memory::kind<memory::global>::release(wksp,wlen);
            }

            inline row & operator[](const coord1D j) throw()
            {
                assert(j>=this->lower.y); assert(j<=this->upper.y);
                return rows[j];
            }

            inline const row & operator[](const coord1D j) const throw()
            {
                assert(j>=this->lower.y); assert(j<=this->upper.y);
                return rows[j];
            }


            const patch1D row_patch;
            type *entry; //!< linear space of this->items

        private:
            row   *rows; //!< allocated rows
            void  *wksp; //!< for this->items * sizeof(T) + width.y * sizeof(row)
            size_t wlen; //!< if allocated
            YOCTO_DISABLE_COPY_AND_ASSIGN(field2D);
            
            inline void link(void        *addr,
                             const size_t data_offset,
                             const size_t rows_offset ) throw()
            {
                assert(addr!=NULL);
                // prepare memory
                {
                    uint8_t *q = static_cast<uint8_t *>(addr);
                    entry      = (type *) &q[data_offset];
                    rows       = (row  *) &q[rows_offset];
                    rows      -= this->lower.y;
                }

                // link rows
                {
                    type *r = entry;
                    for(coord1D j=this->lower.y;j<=this->upper.y;++j,r+=this->width.x)
                    {
                        new (rows+j) row(row_patch,r);
                    }
                }
            }
        };

    }
}

#endif

