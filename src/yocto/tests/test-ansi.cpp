#include "yocto/utest/run.hpp"
#include "yocto/ios/null-device.hpp"
#include "yocto/exceptions.hpp"

using namespace yocto;

YOCTO_UNIT_TEST_IMPL(ansi)
{
    fprintf(stderr,"\x1b[2J"); // clear
    fprintf(stderr,"\x1b[H"); // home
    fprintf(stderr,"\x1b[K"); // erase line
}
YOCTO_UNIT_TEST_DONE()

YOCTO_UNIT_TEST_IMPL(null_device)
{
    ios::null_device::for_cstdin();
    ios::null_device::for_cstdout();
    ios::null_device::for_cstderr();

}
YOCTO_UNIT_TEST_DONE()

