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
    
    {
        const layout_of<coord2d> full( coord2d(1,1), coord2d(16,16) );
        const domain_of<coord2d> dom0(0,1,NULL,full,coord2d(1,1));
        field2d<float> Ff("Ff",dom0,1);

    }

}
YOCTO_UNIT_TEST_DONE()
