#include "yocto/utest/driver.hpp"

YOCTO_UNIT_TEST_INIT(16)
{
    YOCTO_UNIT_TEST_DECL(types);
    YOCTO_UNIT_TEST_DECL(bitmap);
    YOCTO_UNIT_TEST_DECL(pixmaps);
    YOCTO_UNIT_TEST_DECL(fmt);
    YOCTO_UNIT_TEST_DECL(par);
    YOCTO_UNIT_TEST_DECL(ops);

}
YOCTO_UNIT_TEST_EXEC()

