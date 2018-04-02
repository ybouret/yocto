#ifndef YOCTO_IPSO_VISIT_INCLUDED
#define YOCTO_IPSO_VISIT_INCLUDED 1

#include "yocto/visit/interface.hpp"
#include "yocto/ipso/mesh/rectilinear.hpp"
#include "yocto/ipso/mesh/curvilinear.hpp"
#include "yocto/ipso/mesh/point.hpp"

namespace yocto
{
    namespace ipso
    {

        struct visit
        {
            ////////////////////////////////////////////////////////////////////
            //
            // mesh API
            //
            ////////////////////////////////////////////////////////////////////
            template <typename T,const size_t DIM>
            void add_mesh_metadata(visit_handle &md, const rectilinear_mesh<T,DIM> &mesh)
            {
                visit_handle m = VisIt::MeshMetaData_alloc();
                try
                {
                    VisIt_MeshMetaData_setName(mesh,*mesh.name());
                }
                catch(...)
                {
                    VisIt::MeshMetaData_free(m);
                    throw;
                }
            }

        };

    }
}

#endif

