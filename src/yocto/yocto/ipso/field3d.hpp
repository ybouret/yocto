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

#define YOCTO_IPSO_FIELD3D_CTOR(ID)     \
field<T>(ID),                            \
patch3D(p),                               \
slice_patch( p.lower.xy(), p.upper.xy() ), \
slices(0),                                  \
rows(0),                                     \
wksp(0),                                      \
wlen(0)

            inline explicit field3D(const char    *id,
                                    const patch3D &p ) :
            YOCTO_IPSO_FIELD3D_CTOR(id)
            {
                build();
            }

            inline explicit field3D(const string  &id,
                                    const patch3D &p ) :
            YOCTO_IPSO_FIELD3D_CTOR(*id)
            {
                build();
            }

            inline virtual ~field3D() throw()
            {
                for(coord1D k=this->upper.z;k>=this->lower.z;--k)
                {
                    destruct( &slices[k] );
                }
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

            template <typename U>
            inline void load( const field3D<U> &F, const patch3D &zone) throw()
            {
                assert(this->contains(zone));
                for(unit_t z=zone.upper.z;z>=zone.lower.z;--z)
                {
                    const slice &peer_slice=F[z];
                    slice       &self_slice=(*this)[z];
                    for(unit_t y=zone.upper.y;y>=zone.lower.y;--y)
                    {
                        const row &peer_row = peer_slice[y];
                        row       &self_row = self_slice[y];
                        for(unit_t x=zone.upper.x;x>=zone.lower.y;--x)
                        {
                            self_row[x] = static_cast<T>(peer_row[x]);
                        }
                    }
                }
            }

            template <typename U>
            inline void save( field3D<U> &F, const patch3D &zone) const throw()
            {
                assert(this->contains(zone));
                for(unit_t z=zone.upper.z;z>=zone.lower.z;--z)
                {
                    slice       &peer_slice=F[z];
                    const slice &self_slice=(*this)[z];
                    for(unit_t y=zone.upper.y;y>=zone.lower.y;--y)
                    {
                        const row &self_row = self_slice[y];
                        row       &peer_row = peer_slice[y];
                        for(unit_t x=zone.upper.x;x>=zone.lower.y;--x)
                        {
                            peer_row[x] = static_cast<U>(self_row[x]);
                        }
                    }
                }
            }

            inline void load_sub(const field3D &F) throw()
            {
                load<T>(F,F);
            }

            inline void save_sub(field3D &F) const throw()
            {
                save<T>(F,F);
            }

        private:
            slice *slices;
            row   *rows;
            void  *wksp;
            size_t wlen;

            YOCTO_DISABLE_COPY_AND_ASSIGN(field3D);

            inline void build()
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
                    const string slice_id = this->name + vformat("[%d]",int(k));
                    new (&slices[k]) slice(*slice_id,slice_patch,data,r);
                    data += data_per_slice;
                    r    += rows_per_slice;
                }

                this->set_bytes(this->items);
            }
        };
    }
}

#endif

