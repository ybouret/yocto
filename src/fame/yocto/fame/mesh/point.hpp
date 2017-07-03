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
        public domain<COORD>
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            YOCTO_FAME_DECL_COORD;

            typedef layout<COORD>          layout_type;
            typedef domain<COORD>          domain_type;
            typedef domain<coord1d>        axis_domain;
            typedef field1d<T>             axis_type;
            typedef slots_of<axis_type>    holder_type;


            inline virtual ~point_mesh() throw() {}

            inline explicit point_mesh(const string      &tag,
                                       const domain_type &dom,
                                       const coord1d      num_ghosts) :
            mesh_info(tag),
            holder_type(DIMENSION),
            domain_type(dom)
            {
                build_axis(num_ghosts);
            }

            

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(point_mesh);
            inline void build_axis(const coord1d num_ghosts)
            {
                holder_type &hld = *this;
                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                    const axis_domain ax_dom = domain_ops::extract1(*this,dim);
                    const string      ax_tag = this->name + "_" + coord_info::axis_name(dim);
                    hld.template append<const string &,const axis_domain &,coord1d>(ax_tag,ax_dom,num_ghosts);
                    assert(dim+1==hld.size);
                }
            }

        };
    }
}

#endif

