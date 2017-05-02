#include "yocto/spade/layout.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace spade;

YOCTO_UNIT_TEST_IMPL(layout)
{
    std::cerr << "sizeof(layout1D)=" << sizeof(layout1D) << std::endl;
    std::cerr << "sizeof(layout2D)=" << sizeof(layout2D) << std::endl;
    std::cerr << "sizeof(layout3D)=" << sizeof(layout3D) << std::endl;
}
YOCTO_UNIT_TEST_DONE()
