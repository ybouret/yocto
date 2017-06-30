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

            //__________________________________________________________________
            //
            // Meshes API
            //__________________________________________________________________

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
                VisIt_MeshMetaData_setNumDomains(m,rmesh.full.size);
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
                            visit_handle      hx = VisIt::VariableData_Set<T>(X.entry,X.num_outer);
                            const field1d<T> &Y  = rmesh[1];
                            visit_handle      hy = VisIt::VariableData_Set<T>(Y.entry,Y.num_outer);
                            VisIt_RectilinearMesh_setCoordsXY(mesh,hx,hy);
                            VisIt_RectilinearMesh_setRealIndices(mesh,rmesh.imin,rmesh.imax);
                        } break;
                            
                        case 3: {
                            const field1d<T> &X = rmesh[0];
                            visit_handle      hx = VisIt::VariableData_Set<T>(X.entry,X.num_outer);
                            const field1d<T> &Y = rmesh[1];
                            visit_handle      hy = VisIt::VariableData_Set<T>(Y.entry,Y.num_outer);
                            const field1d<T> &Z = rmesh[2];
                            visit_handle      hz = VisIt::VariableData_Set<T>(Z.entry,Z.num_outer);
                            VisIt_RectilinearMesh_setCoordsXYZ(mesh,hx,hy,hz);
                            //VisIt_RectilinearMesh_setRealIndices(mesh,rmesh.imin,rmesh.imax);
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

            //__________________________________________________________________
            //
            // Variables API
            //__________________________________________________________________
            template <typename T,typename COORD,typename MESH> static inline
            visit_handle VariableMetaData( const field<T,COORD> &F, const MESH &mesh )
            {
                visit_handle vmd = VisIt::VariableMetaData_alloc();
                try
                {
                    VisIt_VariableMetaData_setName(vmd,F.name.c_str());
                    VisIt_VariableMetaData_setMeshName(vmd,mesh.name.c_str());
                    VisIt_VariableMetaData_setType(vmd, VisIt::getVariableType<T>() );
                    VisIt_VariableMetaData_setNumComponents(vmd, VisIt::getVariableNumComponents<T>() );
                    VisIt_VariableMetaData_setCentering(vmd, VISIT_VARCENTERING_NODE);
                }
                catch(...)
                {
                    VisIt::VariableMetaData_free(vmd);
                    throw;
                }
                return vmd;
            }

            template <typename T,typename COORD> static inline
            visit_handle VariableData(const field<T,COORD> &F)
            {
                return VisIt::VariableData_Set<T>(F.entry,F.num_outer);
            }

            //__________________________________________________________________
            //
            // Curves API
            //__________________________________________________________________
            template <typename T> static inline
            visit_handle CurveMetaData( const field1d<T> &curve )
            {
                visit_handle cmd = VisIt::CurveMetaData_alloc();
                VisIt_CurveMetaData_setName(cmd, curve.name.c_str() );
                return cmd;
            }

            template <typename T> static inline
            visit_handle CurveData(const field1d<T> &axis,
                                   const field1d<T> &curve )
            {
                assert(axis.outer.items==curve.outer.items);
                visit_handle curv = VisIt::CurveData_alloc();
                try
                {
                    visit_handle hx = VisIt::VariableData_Set(axis.entry,  axis.outer.items);
                    visit_handle hy = VisIt::VariableData_Set(curve.entry,curve.outer.items);
                    VisIt_CurveData_setCoordsXY(curv, hx, hy);
                }
                catch(...)
                {
                    VisIt::CurveData_free(curv);
                    throw;
                }
                return curv;
            }

            

        };

    }

}



#endif

