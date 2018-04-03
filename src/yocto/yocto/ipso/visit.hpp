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
            visit_handle __add_mesh_metadata(visit_handle &md,
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
                return m;
            }

            template <typename T,const size_t DIM> static inline
            visit_handle add_mesh_metadata(visit_handle &md, const rectilinear_mesh<T,DIM> &mesh)
            {
                return __add_mesh_metadata(md,mesh,VISIT_MESHTYPE_RECTILINEAR,mesh.dimension);
            }

            template <typename T,const size_t DIM> static inline
            visit_handle add_mesh_metadata(visit_handle &md, const curvilinear_mesh<T,DIM> &mesh)
            {
                __add_mesh_metadata(md,mesh,VISIT_MESHTYPE_CURVILINEAR,mesh.dimension);
            }

            template <typename T,const size_t DIM> static inline
            visit_handle add_mesh_metadata(visit_handle &md, const point_mesh<T,DIM> &mesh)
            {
                return __add_mesh_metadata(md,mesh,VISIT_MESHTYPE_POINT,1);
            }



            ////////////////////////////////////////////////////////////////////
            //
            // Rectilinear Mesh Data API
            //
            ////////////////////////////////////////////////////////////////////
            static inline visit_handle __rectilinear_mesh_handle()
            {
                visit_handle h = VISIT_INVALID_HANDLE;
                if( VISIT_OKAY != VisIt_RectilinearMesh_alloc(&h) )
                {
                    throw exception("VisIt_RectilinearMesh_alloc");
                }
                return h;
            }


            template <typename T> static inline
            visit_handle get_mesh(const rectilinear_mesh<T,2> &mesh)
            {

                visit_handle h = __rectilinear_mesh_handle();
                try
                {
                    visit_handle hx = VisIt::VariableData_Set( mesh.X().entry, mesh.X().items );
                    visit_handle hy = VisIt::VariableData_Set( mesh.Y().entry, mesh.Y().items );
                    VisIt_RectilinearMesh_setCoordsXY(h,hx,hy);
                }
                catch(...)
                {
                    VisIt_RectilinearMesh_free(h);
                    throw;
                }
                return h;
            }

            template <typename T> static inline
            visit_handle get_mesh(const rectilinear_mesh<T,3> &mesh)
            {

                visit_handle h = __rectilinear_mesh_handle();
                try
                {
                    visit_handle hx = VisIt::VariableData_Set( mesh.X().entry, mesh.X().items );
                    visit_handle hy = VisIt::VariableData_Set( mesh.Y().entry, mesh.Y().items );
                    visit_handle hz = VisIt::VariableData_Set( mesh.Z().entry, mesh.Z().items );
                    VisIt_RectilinearMesh_setCoordsXYZ(h,hx,hy,hz);
                }
                catch(...)
                {
                    VisIt_RectilinearMesh_free(h);
                    throw;
                }
                return h;
            }

            ////////////////////////////////////////////////////////////////////
            //
            // Point Mesh Data API
            //
            ////////////////////////////////////////////////////////////////////
            static inline visit_handle __point_mesh_handle()
            {
                visit_handle h = VISIT_INVALID_HANDLE;
                if( VISIT_OKAY != VisIt_PointMesh_alloc(&h) )
                {
                    throw exception("VisIt_PointMesh_alloc");
                }
                return h;
            }

            template <typename T> static inline
            visit_handle get_mesh(const point_mesh<T,2> &mesh)
            {
                visit_handle h = __point_mesh_handle();
                try
                {
                    visit_handle hx = VisIt::VariableData_Set( mesh.X().entry, mesh.X().items );
                    visit_handle hy = VisIt::VariableData_Set( mesh.Y().entry, mesh.Y().items );
                    VisIt_PointMesh_setCoordsXY(h,hx,hy);
                }
                catch(...)
                {
                    VisIt_PointMesh_free(h);
                    throw;
                }
                return h;
            }

            template <typename T> static inline
            visit_handle get_mesh(const point_mesh<T,3> &mesh)
            {
                visit_handle h = __point_mesh_handle();
                try
                {
                    visit_handle hx = VisIt::VariableData_Set( mesh.X().entry, mesh.X().items );
                    visit_handle hy = VisIt::VariableData_Set( mesh.Y().entry, mesh.Y().items );
                    visit_handle hz = VisIt::VariableData_Set( mesh.Z().entry, mesh.Z().items );
                    VisIt_PointMesh_setCoordsXYZ(h,hx,hy,hz);
                }
                catch(...)
                {
                    VisIt_PointMesh_free(h);
                    throw;
                }
                return h;
            }

        };

    }
}

#endif

