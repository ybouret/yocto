#include "yocto/fame/layout.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace fame;

YOCTO_UNIT_TEST_IMPL(layout)
{
    {
        layout_of<coord1d> L(1,10);
        std::cerr << "L=" << L << std::endl;
    }
    
    {
        layout_of<coord2d> L( coord2d(-5,-6), coord2d(10,12) );
        std::cerr << "L=" << L << std::endl;
    }
    
    {
        layout_of<coord3d> L( coord3d(-5,-6,-8), coord3d(10,12,14) );
        std::cerr << "L=" << L << std::endl;
    }

}
YOCTO_UNIT_TEST_DONE()

