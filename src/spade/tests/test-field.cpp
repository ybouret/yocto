#include "yocto/spade/field1d.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace spade;

YOCTO_UNIT_TEST_IMPL(field)
{
    field1D<double> A(1,10);
    field1D<double> B(1,5,2,2);
}
YOCTO_UNIT_TEST_DONE()
