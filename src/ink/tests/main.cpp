#include "yocto/utest/driver.hpp"

YOCTO_UNIT_TEST_INIT(16)
{
    YOCTO_UNIT_TEST_DECL(types);
    YOCTO_UNIT_TEST_DECL(bitmap);
    YOCTO_UNIT_TEST_DECL(pixmaps);
    YOCTO_UNIT_TEST_DECL(fmt);
    YOCTO_UNIT_TEST_DECL(par);
    YOCTO_UNIT_TEST_DECL(ops);
    YOCTO_UNIT_TEST_DECL(ramp);
    YOCTO_UNIT_TEST_DECL(diff);
    YOCTO_UNIT_TEST_DECL(hist);
    YOCTO_UNIT_TEST_DECL(blob);
    YOCTO_UNIT_TEST_DECL(fg);

    YOCTO_UNIT_TEST_DECL(local);

}
YOCTO_UNIT_TEST_EXEC()

