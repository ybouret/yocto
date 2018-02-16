#include "yocto/utest/driver.hpp"

using namespace yocto;


YOCTO_UNIT_TEST_INIT(16)
{
    
    YOCTO_UNIT_TEST_DECL(ipso);
    YOCTO_UNIT_TEST_DECL(divide);
    YOCTO_UNIT_TEST_DECL(coms);
    YOCTO_UNIT_TEST_DECL(opt);
    YOCTO_UNIT_TEST_DECL(xbuffer);
    YOCTO_UNIT_TEST_DECL(xcom);
    YOCTO_UNIT_TEST_DECL(wksp);
    YOCTO_UNIT_TEST_DECL(vtk);

}
YOCTO_UNIT_TEST_EXEC()

