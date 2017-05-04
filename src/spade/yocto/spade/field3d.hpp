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
            typedef Layout2D            slice_layout;
            typedef Ghosts2D            slice_ghosts;
            typedef Ghost2D             slice_ghost;
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
            entry(0),
            slices(0),
            shift(0)
            {
                
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
