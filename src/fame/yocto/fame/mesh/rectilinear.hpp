#ifndef YOCTO_FAME_MESH_RECTILINEAR_INCLUDED
#define YOCTO_FAME_MESH_RECTILINEAR_INCLUDED 1

#include "yocto/fame/mesh.hpp"
#include "yocto/fame/layouts.hpp"

namespace yocto
{
    namespace fame
    {
        template <typename T,typename COORD>
        class rectilinear_mesh : public mesh_info
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            YOCTO_FAME_DECL_COORD;

            typedef field1d<T>    axis_type;
            typedef layout<COORD> layout_type;

            inline virtual ~rectilinear_mesh() throw() {}
            
            inline explicit rectilinear_mesh(const layout_type &full) :
            _axis(DIMENSION)
            {
            }


        private:
            YOCTO_DISABLE_ASSIGN(rectilinear_mesh);
            slots_of<axis_type> _axis;
        };
    }
}

#endif
