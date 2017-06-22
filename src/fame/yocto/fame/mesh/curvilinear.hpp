#ifndef YOCTO_FAME_MESH_CURVILINEAR_INCLUDED
#define YOCTO_FAME_MESH_CURVILINEAR_INCLUDED 1

#include "yocto/fame/mesh.hpp"
#include "yocto/fame/layouts.hpp"

namespace yocto
{
    namespace fame
    {
        template <typename T,typename COORD>
        class curvilinear_mesh :
        public mesh_info,
        public slots_of< typename field_for<T,COORD>::type >,
        public domain<COORD>
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            YOCTO_FAME_DECL_COORD;

            typedef layout<COORD>                     layout_type;
            typedef domain<COORD>                     domain_type;
            typedef typename field_for<T,COORD>::type coords_type;
            typedef slots_of<coords_type>             holder_type;

            inline virtual ~curvilinear_mesh() throw() {}

            inline explicit curvilinear_mesh(const string      &tag,
                                             const domain_type &dom,
                                             const coord1d      num_ghosts) :
            mesh_info(tag),
            holder_type(DIMENSION),
            domain_type(dom)
            {
                build_coords(num_ghosts);
            }


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(curvilinear_mesh);
            inline void build_coords(const size_t num_ghosts)
            {
                holder_type &self = *this;
                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                    const string coords_tag = this->name + "_" + coord_info::axis_name(dim);
                }
            }

        };
        
    }
    
}

#endif
