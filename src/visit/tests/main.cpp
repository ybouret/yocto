#include "yocto/utest/driver.hpp"

YOCTO_UNIT_TEST_INIT(16)
{
    YOCTO_UNIT_TEST_DECL(loop);
    YOCTO_UNIT_TEST_DECL(ipso);
    YOCTO_UNIT_TEST_DECL(heat1d);

}
YOCTO_UNIT_TEST_EXEC()
