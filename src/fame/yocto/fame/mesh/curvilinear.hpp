#ifndef YOCTO_FAME_MESH_CURVILINEAR_INCLUDED
#define YOCTO_FAME_MESH_CURVILINEAR_INCLUDED 1

#include "yocto/fame/mesh/rectilinear.hpp"
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


            struct __map_info
            {
                const field1d<T> *axis;
                size_t            dim;
            };

            //! regular mapping
            template <typename U>
            inline void map_to(const box<U,COORD> &B) throw()
            {
                curvilinear_mesh<T,COORD>      &cmesh = *this;
                const rectilinear_mesh<T,COORD> rmesh("rmesh",cmesh,cmesh[0].depth,B);
                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                    __map_info params = { &rmesh[dim], dim };
                    cmesh[dim].for_each(cmesh[dim].outer,transfer,&params);
                }
            }


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(curvilinear_mesh);
            inline void build_coords(const coord1d num_ghosts)
            {
                holder_type &hld = *this;
                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                    const string coords_tag = this->name + "_" + coord_info::axis_name(dim);
                    hld. template append<const string&,const domain_type&,coord1d>(coords_tag,*this,num_ghosts);
                    assert(1+dim==hld.size);
                }
            }

            static inline void transfer(type &V, param_coord C, void *args)
            {
                __map_info       *params = static_cast<__map_info*>(args);
                const field1d<T> &axis   = * (params->axis);
                V = axis[ __coord(C,params->dim) ];
            }

        };
        
    }
    
}

#endif
