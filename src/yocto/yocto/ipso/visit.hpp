#ifndef YOCTO_IPSO_VISIT_INCLUDED
#define YOCTO_IPSO_VISIT_INCLUDED 1

#include "yocto/visit/interface.hpp"
#include "yocto/ipso/mesh/rectilinear.hpp"
#include "yocto/ipso/mesh/curvilinear.hpp"
#include "yocto/ipso/mesh/point.hpp"
#include "yocto/ipso/mapping.hpp"

namespace yocto
{
    namespace ipso
    {

        struct __visit
        {
            ////////////////////////////////////////////////////////////////////
            //
            // Mesh Meta Data API
            //
            ////////////////////////////////////////////////////////////////////
            static inline
            void __add_mesh(visit_handle    &m,
                            const mesh_info &mesh)
            {
                YOCTO_MPI_GET();
                const size_t d = mesh.dimension;
                VisIt_MeshMetaData_setName(m,*mesh.name);
                VisIt_MeshMetaData_setSpatialDimension(m,d);
                VisIt_MeshMetaData_setNumDomains(m,MPI.CommWorldSize);
                switch(d)
                {
                    case 3: VisIt_MeshMetaData_setZLabel(m,*mesh[2].name);
                    case 2: VisIt_MeshMetaData_setYLabel(m,*mesh[1].name);
                    case 1: VisIt_MeshMetaData_setXLabel(m,*mesh[0].name);
                    default:
                        break;
                }

            }

            template <typename MESH> static inline
            void __add_mesh_metadata(visit_handle &md,
                                     const MESH   &mesh,
                                     const int     mesh_type,
                                     const int     topo_dims)
            {
                visit_handle m = VisIt::MeshMetaData_alloc();
                try
                {
                    __add_mesh(m,mesh);
                    VisIt_MeshMetaData_setMeshType(m,mesh_type);
                    VisIt_MeshMetaData_setTopologicalDimension(m,topo_dims);
                    VisIt_SimulationMetaData_addMesh(md,m);
                }
                catch(...)
                {
                    VisIt::MeshMetaData_free(m);
                    throw;
                }
            }

            template <typename T,const size_t DIM> static inline
            void add_mesh_metadata(visit_handle &md, const rectilinear_mesh<T,DIM> &mesh)
            {
                __add_mesh_metadata(md,mesh,VISIT_MESHTYPE_RECTILINEAR,mesh.dimension);
            }

            template <typename T,const size_t DIM> static inline
            void add_mesh_metadata(visit_handle &md, const curvilinear_mesh<T,DIM> &mesh)
            {
                __add_mesh_metadata(md,mesh,VISIT_MESHTYPE_CURVILINEAR,mesh.dimension);
            }

            template <typename T,const size_t DIM> static inline
            void add_mesh_metadata(visit_handle &md, const point_mesh<T,DIM> &mesh)
            {
                __add_mesh_metadata(md,mesh,VISIT_MESHTYPE_POINT,1);
            }


        };

    }
}

#endif

