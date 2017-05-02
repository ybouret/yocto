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
            const size_t owned_memory;
            
        protected:
            explicit field_info() throw();
            
        private:
            YOCTO_DISABLE_ASSIGN(field_info);
        };

        template <typename T,typename COORD>
        class field_of : public field_info, public layout_of<COORD>
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            typedef layout_of<COORD>                     layout_type;
            typedef typename    layout_type::coord       coord;
            typedef typename    layout_type::const_coord const_coord;
            typedef typename    layout_type::param_coord param_coord;
            static const size_t DIMENSION = layout_type::DIMENSION;

            inline virtual ~field_of() throw() {}

            const layout_type inner;

            // coordinate API


        protected:
            inline explicit field_of(param_coord lo, param_coord up) :
            field_info(),
            layout_type(lo,up),
            inner(*this)
            {
            }


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(field_of);
        };

    }
}

#endif
