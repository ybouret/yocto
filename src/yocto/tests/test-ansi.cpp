#include "yocto/utest/run.hpp"
#include <cstdio>

using namespace yocto;

YOCTO_UNIT_TEST_IMPL(ansi)
{
    fprintf(stderr,"\x1b[2J"); // clear
    fprintf(stderr,"\x1b[H"); // home
    fprintf(stderr,"\x1b[K"); // erase line

}
YOCTO_UNIT_TEST_DONE()
