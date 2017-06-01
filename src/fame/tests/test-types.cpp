#include "yocto/fame/types.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace fame;

YOCTO_UNIT_TEST_IMPL(types)
{
    coord1d C1 = 1;
    std::cerr << "prod(" << C1 << ")=" << __coord_prod(C1) << std::endl;
    
    coord2d C2(1,2);
    std::cerr << "prod(" << C2 << ")=" << __coord_prod(C2) << std::endl;
    
    coord3d C3(1,2,3);
    std::cerr << "prod(" << C3 << ")=" << __coord_prod(C3) << std::endl;

}
YOCTO_UNIT_TEST_DONE()

