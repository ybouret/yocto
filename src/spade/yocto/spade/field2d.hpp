#ifndef YOCTO_SPADE_FIELD2D_INCLUDED
#define YOCTO_SPADE_FIELD2D_INCLUDED 1

#include "yocto/spade/field1d.hpp"

namespace yocto
{
    namespace spade
    {

        template <typename T>
        class field2D : public field_of<T,coord2D>
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            //__________________________________________________________________
            //
            // 2D types
            //__________________________________________________________________
            typedef layout2D            layout_type;
            typedef ghosts2D            ghosts_type;
            typedef field_of<T,coord2D> field_type;

            //__________________________________________________________________
            //
            // 1D types
            //__________________________________________________________________
            typedef field_of<T,coord1D> row_type;
            typedef layout1D            row_layout;
            typedef ghosts1D            row_ghosts;

            inline explicit field2D(const layout_type &L,
                                    const ghosts_type &G,
                                    void              *E = NULL ) :
            count(0),
            entry(0),
            rows(0)
            {
                
            }

            inline virtual ~field2D() throw() {}


            const void *address_of( typename field_type::param_coord C ) const throw()
            {
                return NULL;
            }




        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(field2D);
            size_t    count;
            type     *entry;
            row_type *rows;

        };

    }
}

#endif
