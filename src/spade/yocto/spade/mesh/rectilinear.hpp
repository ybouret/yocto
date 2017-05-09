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

        //! a rectilinear mesh
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
            typedef box_of<T,COORD>  box_type;

            static const size_t DIMENSION = sizeof(COORD)/sizeof(coord1D);

            inline virtual ~RectilinearMesh() throw() {}

            inline explicit RectilinearMesh(const string      &id,
                                            const layout_type &L,
                                            const ghosts_type &G,
                                            const box_type    *pBox = 0
                                            ) :
            Mesh(id,DIMENSION),
            axis__(DIMENSION,as_capacity),
            pAxis(0)
            {
                build_with(L,G);
                if(pBox)
                {
                    map_inner(*pBox);
                }
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

            inline void build_with(const layout_type &L,const ghosts_type &G)
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

            inline void map_inner( const box_type &B) throw()
            {
                RectilinearMesh &self = *this;
                const_type     *lower = B.__lower();
                const_type     *width = B.__width();
                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                    const_type umin    = lower[dim];
                    const_type udif    = width[dim];
                    Axis      &U       = self[dim];
                    const coord1D ini  = U.outer.lower;
                    const coord1D end  = U.outer.upper;
                    const coord1D imin = U.inner.lower;
                    const coord1D imax = U.inner.upper;
                    const coord1D idif = imax-imin;

                    if(idif<=0)
                    {
                        const_type *upper = B.__upper();
                        const_type  umax  = upper[dim];
                        U[imin] = (umin+umax)/2;
                    }
                    else
                    {
                        for(coord1D p=ini;p<=end;++p)
                        {
                            U[p] = umin + (type((p-imin) * udif))/idif;
                        }
                    }
                }
            }

        };


    }
}

#endif
