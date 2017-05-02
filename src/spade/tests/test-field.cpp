#include "yocto/spade/field2d.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace spade;

YOCTO_UNIT_TEST_IMPL(field)
{
    {
        field1D<double> A(1,10);
        field1D<double> B(1,5,2,2);
    }


    {
        field2D<float> A( coord2D(0,1), coord2D(9,10) );
        field2D<float> B( coord2D(1,1), coord2D(10,10), coord2D(1,3), coord2D(1,3) );
    }
}
YOCTO_UNIT_TEST_DONE()
