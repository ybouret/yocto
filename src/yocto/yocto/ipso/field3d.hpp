#ifndef YOCTO_IPSO_FIELD3D_INCLUDED
#define YOCTO_IPSO_FIELD3D_INCLUDED 1

#include "yocto/ipso/field2d.hpp"

namespace yocto
{
    namespace ipso
    {
        template <typename T>
        class field3D : public field<T>, public patch3D
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            typedef field1D<T> row;
            typedef field2D<T> slice;

            inline explicit field3D( const patch3D &p ) :
            field<T>(),
            patch3D(p),
            slice_patch( coord2D(p.lower.x,p.lower.y), coord2D(p.upper.x,p.upper.y) ),
            wksp(0),
            wlen(0)
            {
                // compute required memory
                const size_t data_offset   = 0;
                const size_t data_length   = this->items * sizeof(type);
                const size_t slices_offset = memory::align( data_offset+data_length );
                const size_t slices_length = this->width.z * sizeof(slice);
                const size_t rows_offset   = memory::align( slices_offset+slices_length );
                const size_t rows_length   = sizeof(row) * this->width.z * this->width.y;

                // allocate and map memory
                wlen = memory::align(rows_offset+rows_length);
                wksp = memory::kind<memory::global>::acquire(wlen);
                uint8_t *q = static_cast<uint8_t *>(wksp);

                this->entry  = (type  *) &q[ data_offset   ];
                slices       = (slice *) &q[ slices_offset ];
                rows         = (row   *) &q[ rows_offset   ];
                slices      -= this->lower.z;

                // link memory
                type         *data           = this->entry;
                const coord1D data_per_slice = this->pitch.z;
                row          *r              = rows;
                const coord1D rows_per_slice = this->width.y;

                for(coord1D k=this->lower.z;k<=this->upper.z;++k)
                {
                    new (slices+k) slice(slice_patch,data,r);
                    data += data_per_slice;
                    rows += rows_per_slice;
                }

                this->set_bytes(this->items);


            }

            inline virtual ~field3D() throw()
            {
                memory::kind<memory::global>::release(wksp,wlen);
            }

            inline slice & operator[](const coord1D k) throw()
            {
                assert(k>=this->lower.z);
                assert(k<=this->upper.z);
                return slices[k];
            }

            inline const slice & operator[](const coord1D k) const throw()
            {
                assert(k>=this->lower.z);
                assert(k<=this->upper.z);
                return slices[k];
            }

            const patch2D slice_patch;

        private:
            slice *slices;
            row   *rows;
            void  *wksp;
            size_t wlen;

            YOCTO_DISABLE_COPY_AND_ASSIGN(field3D);
        };
    }
}

#endif

