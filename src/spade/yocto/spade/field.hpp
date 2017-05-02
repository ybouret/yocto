#ifndef YOCTO_SPADE_FIELD_INCLUDED
#define YOCTO_SPADE_FIELD_INCLUDED 1

#include "yocto/spade/layout.hpp"

namespace yocto
{
    namespace spade
    {

        class field_info : public object
        {
        public:
            virtual ~field_info() throw();

        protected:
            explicit field_info() throw();


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(field_info);
        };

        template <typename T,typename COORD>
        class field_of : public layout_of<COORD>
        {
        public:
            YOCTO_SPADE_DECL_COORD();
            typedef layout_of<COORD> layout_type;

            const layout_type inner;


        protected:
            explicit field_of(param_coord lo,
                              param_coord hi) :
            layout_type(lo,hi),
            inner(this)
            {
            }

            explicit field_of(param_coord lo,
                              param_coord up,
                              param_coord ghost_lo,
                              param_coord ghost_up) :
            layout_type(__coord_lower_of(lo,up)-__coord_abs(ghost_lo),__coord_upper_of(lo,up)+__coord_abs(ghost_up)),
            inner(lo,up)
            {

            }


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(field_of);
        };
        


    }
}

#endif
