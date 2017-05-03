#include "yocto/spade/field2d.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace spade;

YOCTO_UNIT_TEST_IMPL(field)
{
    {
        const layout1D  L_A(1,10);
        const ghost1D   no_ghost(0);
        const ghosts1D  no_ghosts(0,no_ghost,no_ghost);
        field1D<double> A(L_A,no_ghosts);
        const ghost1D   g1(0,1);
        const ghost1D   g2(0,2);
        const ghosts1D  gs1(0,g1,g2);

        field1D<double> B(L_A,gs1);
    }


#if 0
    {
        field2D<float> A( coord2D(0,1), coord2D(9,10) );
        field2D<float> B( coord2D(1,1), coord2D(10,10), coord2D(1,3), coord2D(1,3) );
    }
#endif
    
}
YOCTO_UNIT_TEST_DONE()
