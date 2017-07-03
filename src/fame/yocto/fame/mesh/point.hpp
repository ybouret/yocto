#ifndef YOCTO_FAME_MESH_POINT_INCLUDED
#define YOCTO_FAME_MESH_POINT_INCLUDED 1

#include "yocto/fame/mesh.hpp"
#include "yocto/fame/layouts.hpp"

namespace yocto
{
    namespace fame
    {
        template <typename T,typename COORD>
        class point_mesh :
        public mesh_info,
        public slots_of< field1d<T> >,
        public domain<coord1d>
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            YOCTO_FAME_DECL_COORD;
            
            typedef layout<COORD>          layout_type;
            typedef domain<coord1d>        domain_type;
            typedef field1d<T>             coords_type;
            typedef slots_of<coords_type>  holder_type;
            
            
            inline virtual ~point_mesh() throw() {}
            
            inline explicit point_mesh(const string      &tag,
                                       const domain_type &dom,
                                       const coord1d      num_ghosts) :
            mesh_info(tag),
            holder_type(DIMENSION),
            domain_type(dom)
            {
                build_coords(num_ghosts);
            }
            
            
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(point_mesh);
            inline void build_coords(const coord1d num_ghosts)
            {
                holder_type &hld = *this;
                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                    const string      ax_tag = this->name + "_" + coord_info::axis_name(dim);
                    hld.template append<const string &,const domain_type &,coord1d>(ax_tag,*this,num_ghosts);
                    assert(dim+1==hld.size);
                }
            }
            
        };
    }
}

#endif

