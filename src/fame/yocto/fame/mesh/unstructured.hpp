#ifndef YOCTO_FAME_MESH_UNSTRUCTURED_INCLUDED
#define YOCTO_FAME_MESH_UNSTRUCTURED_INCLUDED 1

#include "yocto/fame/mesh/point.hpp"
#include "yocto/fame/layouts.hpp"

namespace yocto
{
    namespace fame
    {


#define FAME_CELL_BEAM                   0
#define FAME_CELL_TRI                    1
#define FAME_CELL_QUAD                   2
#define FAME_CELL_TET                    3
#define FAME_CELL_PYR                    4
#define FAME_CELL_WEDGE                  5
#define FAME_CELL_HEX                    6
#define FAME_CELL_POINT                  7
#define FAME_CELL_POLYHEDRON             8


        template <typename T,typename COORD>
        class unstructured_mesh : public point_mesh<T,COORD>
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            YOCTO_FAME_DECL_COORD;

            typedef layout<COORD>          layout_type;
            typedef domain<coord1d>        domain_type;
            typedef field1d<T>             coords_type;

            inline virtual ~unstructured_mesh() throw() {}
            
            inline explicit unstructured_mesh(const string      &tag,
                                              const domain_type &dom,
                                              const coord1d      num_ghosts) :
            point_mesh<T,COORD>(tag,dom,num_ghosts),
            nCells(0),
            connectivity()
            {
            }

            inline int indx2conn(const coord1d indx) const throw()
            {
                assert( (*this)[0].outer.has(indx) );

                return indx-(*this)[0].outer.lower;
            }

            inline void tri(const coord1d i0, const coord1d i1, const coord1d i2)
            {
                connectivity.push_back(FAME_CELL_TRI);
                connectivity.push_back(indx2conn(i0));
                connectivity.push_back(indx2conn(i1));
                connectivity.push_back(indx2conn(i2));
                ++nCells;
            }

            inline void point(coord1d i0)
            {
                connectivity.push_back(FAME_CELL_POINT);
                connectivity.push_back(indx2conn(i0));
                ++nCells;
            }

            inline int          num_cells()  const throw() { return nCells; }
            inline const int   *conn_addr()  const throw() { return connectivity();      }
            inline size_t       conn_size()  const throw() { return connectivity.size(); }
        protected:
            int         nCells;
            vector<int> connectivity;


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(unstructured_mesh);

        };
    }
}

#endif

