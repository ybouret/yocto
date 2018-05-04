#include "yocto/utest/driver.hpp"

YOCTO_UNIT_TEST_INIT(16)
{
    YOCTO_UNIT_TEST_DECL(library);
    YOCTO_UNIT_TEST_DECL(eqs);
    YOCTO_UNIT_TEST_DECL(balance);
    YOCTO_UNIT_TEST_DECL(norm);
    YOCTO_UNIT_TEST_DECL(lua);

}
YOCTO_UNIT_TEST_EXEC()

