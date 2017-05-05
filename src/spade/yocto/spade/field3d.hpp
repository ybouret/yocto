#ifndef YOCTO_SPADE_FIELD3D_INCLUDED
#define YOCTO_SPADE_FIELD3D_INCLUDED 1

#include "yocto/spade/field2d.hpp"

namespace yocto
{
    namespace spade
    {

        template <typename T>
        class Field3D : public field_of<T,coord3D>
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            //__________________________________________________________________
            //
            // 3D types
            //__________________________________________________________________
            typedef Layout3D            layout_type;
            typedef Ghosts3D            ghosts_type;
            typedef field_of<T,coord3D> field_type;

            //__________________________________________________________________
            //
            // 2D types
            //__________________________________________________________________
            //typedef Layout2D            slice_layout;
            //typedef Ghosts2D            slice_ghosts;
            //typedef Ghost2D             slice_ghost;
            typedef Field2D<T>          slice;

            virtual ~Field3D() throw()
            {

            }

            explicit Field3D(const string      &id,
                             const layout_type &L,
                             const ghosts_type &G) :
            field_type(id,L,G),
            buflen(0),
            buffer(0),
            slices(0),
            shift(0)
            {
#if 0
                const size_t   num_slices  = this->outer.width.z;
                const Layout2D slice_layout( coord2D(L.lower.x,L.lower.y), coord2D(L.upper.x,L.upper.y) );
                const Ghost2D  slice_lower_ghost( coord2D(G.lower.peer.x,G.lower.peer.y), coord2D(G.lower.size.x,G.lower.size.y) );
                const Ghost2D  slice_upper_ghost( coord2D(G.upper.peer.x,G.upper.peer.y), coord2D(G.upper.size.x,G.upper.size.y) );
                const Ghosts2D slice_ghosts(G.rank,slice_lower_ghost,slice_upper_ghost);
                const size_t   bytes_per_slice = slice::ComputeBufferSize(slice_layout);
                const size_t   slices_offset   = 0;
                const size_t   slices_length   = num_slices * sizeof(slice);
                const size_t   data_offset     = memory::align(slices_offset+slices_length);
                const size_t   data_length     = num_slices * bytes_per_slice;

                buflen = memory::align(data_offset+data_length);
                buffer = memory::kind<memory::global>::acquire(buflen);

                uint8_t *p = (uint8_t*)buffer;
                slices     = (slice  *) &p[slices_offset];
                shift      = slices-this->outer.lower.z;
#endif
            }


            inline virtual const void *address_of( typename field_type::param_coord C ) const throw()
            {
                assert(this->outer.has(C));
                return &shift[C.z][C.y][C.x];
            }

            inline virtual const void * item_addr(const size_t indx) const throw()
            {
                assert(indx<this->outer.items);
                return &entry[indx];
            }


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Field3D);
            size_t buflen;
            void  *buffer;
            type  *entry;
            slice *slices;
            slice *shift;
        };

    }

}

#endif
