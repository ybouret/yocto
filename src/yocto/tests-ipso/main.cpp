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
    YOCTO_UNIT_TEST_DECL(stl);
    YOCTO_UNIT_TEST_DECL(mesh);
    YOCTO_UNIT_TEST_DECL(onesub);

}
YOCTO_UNIT_TEST_EXEC()

