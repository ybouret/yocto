#ifndef YOCTO_FAME_MESH_UNSTRUCTURED_INCLUDED
#define YOCTO_FAME_MESH_UNSTRUCTURED_INCLUDED 1

#include "yocto/fame/mesh/point.hpp"
#include "yocto/fame/layouts.hpp"

namespace yocto
{
    namespace fame
    {
        template <typename T,typename COORD>
        class unstructured_mesh : public point_mesh<T,COORD>
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            YOCTO_FAME_DECL_COORD;
            
            typedef layout<COORD>          layout_type;
            typedef domain<coord1d>        domain_type;
            typedef field1d<T>             coords_type;
            vector<int>                    connectivity;
            
            inline virtual ~unstructured_mesh() throw() {}
            
            inline explicit unstructured_mesh(const string      &tag,
                                              const domain_type &dom,
                                              const coord1d      num_ghosts) :
            point_mesh<T,COORD>(tag,dom,num_ghosts)
            {
            }
            
            
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(unstructured_mesh);
        };
    }
}

#endif

