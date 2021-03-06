#include "yocto/utest/driver.hpp"

YOCTO_UNIT_TEST_INIT(16)
{
    YOCTO_UNIT_TEST_DECL(library);
    YOCTO_UNIT_TEST_DECL(equilibrium);
    YOCTO_UNIT_TEST_DECL(equilibria);
    YOCTO_UNIT_TEST_DECL(boot);
    YOCTO_UNIT_TEST_DECL(ode);
    YOCTO_UNIT_TEST_DECL(titration);
    YOCTO_UNIT_TEST_DECL(lua);
    YOCTO_UNIT_TEST_DECL(var);
    YOCTO_UNIT_TEST_DECL(eff);

}
YOCTO_UNIT_TEST_EXEC()
