#ifndef YOCTO_FAME_VISIT_INCLUDED
#define YOCTO_FAME_VISIT_INCLUDED 1

#include "yocto/visit/interface.hpp"
#include "yocto/fame/mesh/rectilinear.hpp"

namespace yocto
{

    namespace fame
    {

        struct __visit
        {

            template <typename QUAD_MESH> static inline
            void QuadMeshMetaData(visit_handle    &m,
                                  const QUAD_MESH &mesh)
            {
                VisIt_MeshMetaData_setName(m, mesh.name.c_str() );
                VisIt_MeshMetaData_setTopologicalDimension(m, QUAD_MESH::DIMENSION);
                VisIt_MeshMetaData_setSpatialDimension(m,     QUAD_MESH::DIMENSION);
            }
            
            template <typename T,typename COORD>
            static inline
            visit_handle MeshMetaData( const rectilinear_mesh<T,COORD> &rmesh )
            {
                visit_handle m = VisIt::MeshMetaData_alloc();
                QuadMeshMetaData(m,rmesh);
                VisIt_MeshMetaData_setMeshType(m,VISIT_MESHTYPE_RECTILINEAR);
                return m;
            }

            template <typename T,typename COORD>
            static inline
            visit_handle MeshData( const rectilinear_mesh<T,COORD> &rmesh )
            {
                visit_handle mesh = VISIT_INVALID_HANDLE;
                if(VISIT_OKAY!=VisIt_RectilinearMesh_alloc(&mesh))
                {
                    throw exception("VisIt_RectilinearMesh_alloc");
                }
                
                try
                {
                    switch(rmesh.DIMENSION)
                    {
                        case 2: {
                            const field1d<T> &X  = rmesh[0];
                            visit_handle      hx = VisIt::VariableData_Set<T>(X.get_outer(),X.num_outer);
                            const field1d<T> &Y  = rmesh[1];
                            visit_handle      hy = VisIt::VariableData_Set<T>(Y.get_outer(),Y.num_outer);
                            VisIt_RectilinearMesh_setCoordsXY(mesh,hx,hy);
                            VisIt_RectilinearMesh_setRealIndices(mesh,rmesh.imin,rmesh.imax);
                        } break;
                            
                        case 3: {
                            const field1d<T> &X = rmesh[0];
                            visit_handle      hx = VisIt::VariableData_Set<T>(X.get_outer(),X.num_outer);
                            const field1d<T> &Y = rmesh[1];
                            visit_handle      hy = VisIt::VariableData_Set<T>(Y.get_outer(),Y.num_outer);
                            const field1d<T> &Z = rmesh[2];
                            visit_handle      hz = VisIt::VariableData_Set<T>(Z.get_outer(),Z.num_outer);
                            VisIt_RectilinearMesh_setCoordsXYZ(mesh,hx,hy,hz);
                            VisIt_RectilinearMesh_setRealIndices(mesh,rmesh.imin,rmesh.imax);
                        } break;
                            
                        default:
                            throw exception("MeshData: invalid RectilinearMesh::DIMENSION=%u", unsigned(rmesh.DIMENSION) );
                    }
                }
                catch(...)
                {
                    VisIt_RectilinearMesh_free(mesh);
                    throw;
                }

                
                assert(mesh!=VISIT_INVALID_HANDLE);
                return mesh;
                
            }

            
            
        };

    }

}



#endif

