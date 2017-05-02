#include "yocto/spade/types.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace spade;

YOCTO_UNIT_TEST_IMPL(types)
{
    std::cerr << "sizeof(coord1D)=" << sizeof(coord1D) << std::endl;
    std::cerr << "sizeof(coord2D)=" << sizeof(coord2D) << std::endl;
    std::cerr << "sizeof(coord3D)=" << sizeof(coord3D) << std::endl;
}
YOCTO_UNIT_TEST_DONE()
