#include "yocto/spade/mesh/rectilinear.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace spade;

YOCTO_UNIT_TEST_IMPL(rmesh)
{
    {
        const Layout1D L(-5,8);
        const Ghost1D  lower_ghost(-10,1);
        const Ghost1D  upper_ghost(10,2);
        const Ghosts1D G(-1,lower_ghost,upper_ghost);
        RectilinearMesh<double,coord1D> rm("mesh1d",L,G);

        Field1D<double> &X = rm[0];
        for(coord1D i=X.outer.lower;i<=X.outer.upper;++i)
        {
            X[i] = double(i);
        }
    }

    {
        const Layout2D L( coord2D(1,1), coord2D(10,5)  );
        const Ghost2D  g_lo(coord2D(11,12), coord2D(1,2) );
        const Ghost2D  g_up(coord2D(13,14), coord2D(1,2) );
        const Ghosts2D G(0,g_lo,g_up);
        RectilinearMesh<float,coord2D> rm("mesh2d",L,G);
        for(size_t dim=0;dim<2;++dim)
        {
            Field1D<float> &U = rm[dim];
            for(coord1D i=U.outer.lower;i<=U.outer.upper;++i)
            {
                U[i] = float(i);
            }
        }

    }
}
YOCTO_UNIT_TEST_DONE()

