#ifndef YOCTO_SPADE_FIELD1D_INCLUDED
#define YOCTO_SPADE_FIELD1D_INCLUDED 1

#include "yocto/spade/field.hpp"

namespace yocto
{
    namespace spade
    {
        template <typename T>
        class field1D : public field_of<T,coord1D>
        {
        public:
            typedef field_of<T,coord1D> field_type;

            virtual ~field1D() throw() {}

            explicit field1D(coord1D lo, coord1D up) :
            field_type(lo,up)
            {
            }

            explicit field1D(coord1D lo, coord1D up, coord1D ghost_lo, coord1D ghost_up) :
            field_type(lo,up,ghost_lo,ghost_up)
            {
            }
            


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(field1D);
        };
    }
}

#endif
