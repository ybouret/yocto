#include "yocto/spade/layout.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace spade;

YOCTO_UNIT_TEST_IMPL(layout)
{
    std::cerr << "sizeof(layout1D)=" << sizeof(layout1D) << std::endl;
    std::cerr << "sizeof(layout2D)=" << sizeof(layout2D) << std::endl;
    std::cerr << "sizeof(layout3D)=" << sizeof(layout3D) << std::endl;

    {
        layout1D L(10,-1);
        std::cerr << "Layout1D=" << L << std::endl;
    }

    {
        layout2D L( coord2D(5,3), coord2D(-3,-2) );
        std::cerr << "Layout2D=" << L << std::endl;
    }

    {
        layout3D L( coord3D(5,3,-4), coord3D(-3,-2,8) );
        std::cerr << "Layout3D=" << L << std::endl;
    }
}
YOCTO_UNIT_TEST_DONE()
