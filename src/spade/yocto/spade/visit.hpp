#ifndef YOCTO_SPADE_VISIT_INCLUDED
#define YOCTO_SPADE_VISIT_INCLUDED 1

#include "yocto/visit/interface.hpp"
#include "yocto/spade/mesh/rectilinear.hpp"

namespace yocto
{

    namespace spade
    {

        struct __visit
        {

            //common
            static inline
            void QuadMeshMetaData(visit_handle &m,
                                  const Mesh   &mesh)
            {
                VisIt_MeshMetaData_setName(m, mesh.name.c_str() );
                VisIt_MeshMetaData_setTopologicalDimension(m, mesh.dimension);
                VisIt_MeshMetaData_setSpatialDimension(m,mesh.dimension);
            }

            template <typename T,typename COORD>
            static inline
            visit_handle MeshMetaData( const RectilinearMesh<T,COORD> &rmesh )
            {
                visit_handle m = VisIt::MeshMetaData_alloc();
                QuadMeshMetaData(m,rmesh);
                VisIt_MeshMetaData_setMeshType(m, VISIT_MESHTYPE_RECTILINEAR);
                return m;
            }

            template <typename T, typename COORD>
            static inline
            visit_handle MeshData( const RectilinearMesh<T,COORD> &rmesh )
            {

                visit_handle mesh = VISIT_INVALID_HANDLE;
                if(VISIT_OKAY!=VisIt_RectilinearMesh_alloc(&mesh))
                {
                    throw exception("VisIt_RectilinearMesh_alloc");
                }

                try
                {
                    switch(rmesh.dimension)
                    {
                        case 2: {
                            const Field1D<T> &X = rmesh[0];
                            visit_handle      hx = VisIt::VariableData_Set<T>(X.base(),X.tuples());
                            const Field1D<T> &Y = rmesh[1];
                            visit_handle      hy = VisIt::VariableData_Set<T>(Y.base(),Y.tuples());
                            VisIt_RectilinearMesh_setCoordsXY(mesh,hx,hy);
                        } break;

                        case 3: {

                        } break;

                        default:
                            throw exception("MeshData: invalid RectilinearMesh::DIMENSION=%u", unsigned(rmesh.dimension) );
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
