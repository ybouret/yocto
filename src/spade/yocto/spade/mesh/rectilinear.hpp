#ifndef YOCTO_SPADE_MESH_RECTILINEAR_INCLUDED
#define YOCTO_SPADE_MESH_RECTILINEAR_INCLUDED 1

#include "yocto/spade/mesh.hpp"
#include "yocto/spade/field1d.hpp"
#include "yocto/spade/ghosts.hpp"
#include "yocto/ptr/arc.hpp"
#include "yocto/sequence/vector.hpp"

namespace yocto
{
    namespace spade
    {

        template <typename T,typename COORD>
        class RectilinearMesh : public Mesh
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            typedef Field1D<type>    Axis;
            typedef arc_ptr<Axis>    AxisPointer;
            typedef Ghost1D          axis_ghost;
            typedef Ghosts1D         axis_ghosts;
            typedef Layout1D         axis_layout;

            typedef layout_of<COORD> layout_type;
            typedef ghost_of<COORD>  ghost_type;
            typedef ghosts_of<COORD> ghosts_type;


            static const size_t DIMENSION = sizeof(COORD)/sizeof(coord1D);

            inline virtual ~RectilinearMesh() throw() {}

            inline explicit RectilinearMesh(const string      &id,
                                            const layout_type &L,
                                            const ghosts_type &G
                                            ) :
            Mesh(id,DIMENSION),
            axis__(DIMENSION,as_capacity)
            {

                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                    const coord1D lower = __coord(L.lower,dim);
                    const coord1D upper = __coord(L.upper,dim);
                }

            }
            

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(RectilinearMesh);
            vector<AxisPointer> axis__;

        };


    }
}

#endif
