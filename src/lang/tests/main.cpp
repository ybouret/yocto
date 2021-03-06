#include "yocto/utest/driver.hpp"

YOCTO_UNIT_TEST_INIT(16)
{
    YOCTO_UNIT_TEST_DECL(module);
    YOCTO_UNIT_TEST_DECL(source);
    YOCTO_UNIT_TEST_DECL(pattern);
    YOCTO_UNIT_TEST_DECL(regexp);
    YOCTO_UNIT_TEST_DECL(posix);
    YOCTO_UNIT_TEST_DECL(types);
    YOCTO_UNIT_TEST_DECL(scan);
    YOCTO_UNIT_TEST_DECL(trans);
    YOCTO_UNIT_TEST_DECL(gram);
    YOCTO_UNIT_TEST_DECL(parser);
    YOCTO_UNIT_TEST_DECL(json);
    YOCTO_UNIT_TEST_DECL(dyn);
    YOCTO_UNIT_TEST_DECL(match);
    
}
YOCTO_UNIT_TEST_EXEC()
