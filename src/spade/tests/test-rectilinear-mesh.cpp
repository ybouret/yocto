#include "yocto/spade/mesh/rectilinear.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace spade;
#include <cstring>

YOCTO_UNIT_TEST_IMPL(rmesh)
{
    {
        const Layout1D L(-5,8);
        const Ghost1D  no_ghost;
        const Ghosts1D G(-1,no_ghost,no_ghost);
        RectilinearMesh<double,coord1D> rm("mesh1d",L,G);

        Field1D<double> &X = rm[0];
        for(coord1D i=X.outer.lower;i<=X.outer.upper;++i)
        {
            X[i] = double(i);
        }


    }
}
YOCTO_UNIT_TEST_DONE()

