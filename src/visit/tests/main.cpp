#include "yocto/utest/driver.hpp"

YOCTO_UNIT_TEST_INIT(16)
{
    YOCTO_UNIT_TEST_DECL(loop);
    YOCTO_UNIT_TEST_DECL(mesh);
    YOCTO_UNIT_TEST_DECL(heat);

}
YOCTO_UNIT_TEST_EXEC()
