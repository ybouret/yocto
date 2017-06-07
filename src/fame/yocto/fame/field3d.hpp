#ifndef YOCTO_FAME_FIELD3D_INCLUDED
#define YOCTO_FAME_FIELD3D_INCLUDED 1

#include "yocto/fame/field2d.hpp"

namespace yocto
{

    namespace fame
    {

        template <typename T>
        class field3d : public field<T,coord3d>
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            typedef field<T,coord3d>                 field_type;
            typedef domain<coord3d>                  domain_type;
            typedef typename field_type::param_coord param_coord;
            typedef field2d<T>                       slice;
            typedef field1d<T>                       row;


            inline virtual ~field3d() throw()
            {
                clear();
            }


        private:
            YOCTO_DISABLE_ASSIGN(field3d);
            slice *slices;
            size_t ns;
            
            inline void clear() throw()
            {
            }

        };

    }
}

#endif
