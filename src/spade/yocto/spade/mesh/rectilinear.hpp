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
            typedef Ghost1D          AxisGhost;
            typedef Ghosts1D         AxisGhosts;
            typedef Layout1D         AxisLayout;

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
            axis__(DIMENSION,as_capacity),
            pAxis(0)
            {
                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                    const AxisLayout  axis_layout = layout_ops::extract<layout_type>(L,dim);
                    const AxisGhosts  axis_ghosts = ghosts_ops::extract<ghosts_type>(G,dim);
                    const string      axis_name   = name + coord_data::axis_name(dim);
                    AxisPointer       axis_ptr( new Axis(axis_name,axis_layout,axis_ghosts) );
                    axis__.push_back(axis_ptr);
                }
                pAxis = &axis__[1];
            }

            inline Axis & operator[](const size_t dim) throw()
            {
                assert(dim<DIMENSION);
                assert(pAxis!=NULL);
                return *pAxis[dim];
            }

            inline const Axis & operator[](const size_t dim) const throw()
            {
                assert(dim<DIMENSION);
                assert(pAxis!=NULL);
                return *pAxis[dim];
            }


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(RectilinearMesh);
            vector<AxisPointer> axis__;
            AxisPointer        *pAxis;
        };


    }
}

#endif
