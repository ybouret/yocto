#ifndef YOCTO_SPADE_FIELD2D_INCLUDED
#define YOCTO_SPADE_FIELD2D_INCLUDED 1

#include "yocto/spade/field1d.hpp"

namespace yocto
{
    namespace spade
    {

#if 0
        template <typename T>
        class field2D : public field_of<T,coord2D>
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            typedef field_of<T,coord2D> field_type;
            typedef field_of<T,coord1D> row_type;



            virtual ~field2D() throw() {}

            explicit field2D(coord2D lo,
                             coord2D up,
                             coord2D ghost_lo = coord_info<coord2D>::zero,
                             coord2D ghost_up = coord_info<coord2D>::zero,
                             type   *usr_entry = NULL) :
            field_type(lo,up,ghost_lo,ghost_up),
            count(0),
            entry(usr_entry),
            rows(0)
            {
                const coord1D row_lower_coord = this->lower.x;
                const coord1D row_upper_coord = this->upper.x;
                const coord1D row_lower_ghost = this->lower_ghost.x;
                const coord1D row_upper_ghost = this->upper_ghost.x;

            }

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
#endif
        
    }
}

#endif
