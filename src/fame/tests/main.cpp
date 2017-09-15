#include "yocto/utest/driver.hpp"

YOCTO_UNIT_TEST_INIT(16)
{
    YOCTO_UNIT_TEST_DECL(types);
    YOCTO_UNIT_TEST_DECL(layout);
    YOCTO_UNIT_TEST_DECL(split);
    YOCTO_UNIT_TEST_DECL(domain);
    YOCTO_UNIT_TEST_DECL(domains);
    YOCTO_UNIT_TEST_DECL(layouts);
    YOCTO_UNIT_TEST_DECL(field);
    YOCTO_UNIT_TEST_DECL(rmesh);
    YOCTO_UNIT_TEST_DECL(cmesh);
    YOCTO_UNIT_TEST_DECL(stl);
    YOCTO_UNIT_TEST_DECL(tr2d);

}
YOCTO_UNIT_TEST_EXEC()

