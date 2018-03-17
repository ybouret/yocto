#include "yocto/utest/driver.hpp"

using namespace yocto;


YOCTO_UNIT_TEST_INIT(16)
{
    
    YOCTO_UNIT_TEST_DECL(ipso);
    YOCTO_UNIT_TEST_DECL(divide);
    YOCTO_UNIT_TEST_DECL(wksp);
    YOCTO_UNIT_TEST_DECL(vtk);
    YOCTO_UNIT_TEST_DECL(subset);
    YOCTO_UNIT_TEST_DECL(subopt);
    YOCTO_UNIT_TEST_DECL(rmesh);
    YOCTO_UNIT_TEST_DECL(pmesh);
    YOCTO_UNIT_TEST_DECL(cmesh);

}
YOCTO_UNIT_TEST_EXEC()

