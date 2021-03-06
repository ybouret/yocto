#include "yocto/utest/driver.hpp"

YOCTO_UNIT_TEST_INIT(32)
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
    YOCTO_UNIT_TEST_DECL(fft);

    YOCTO_UNIT_TEST_DECL(draw);
    YOCTO_UNIT_TEST_DECL(mask);
    YOCTO_UNIT_TEST_DECL(blur);
    YOCTO_UNIT_TEST_DECL(dct);

    YOCTO_UNIT_TEST_DECL(stencil);
    YOCTO_UNIT_TEST_DECL(edges);
    YOCTO_UNIT_TEST_DECL(stengrad);
    YOCTO_UNIT_TEST_DECL(grads);
    YOCTO_UNIT_TEST_DECL(edm);

}
YOCTO_UNIT_TEST_EXEC()

