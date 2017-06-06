#include "yocto/fame/field2d.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace fame;

YOCTO_UNIT_TEST_IMPL(field)
{

    {
        const layout_of<coord1d> full(1,16);
        const domain_of<coord1d> dom0(0,1,NULL,full,1);
        field1d<int> Fi("Fi",dom0,0);
        field1d<int> FiCopy(Fi);
        
    }

}
YOCTO_UNIT_TEST_DONE()
