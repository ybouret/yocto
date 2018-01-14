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
    stdout = freopen(YOCTO_NULL_DEVICE,"w",stdout);
    const int err = ferror(stdout);
    if(err!=0)
    {
        throw libc::exception(err,"freopen(%s)\n",YOCTO_NULL_DEVICE);
    }
    if( fprintf(stdout,"Hello, World!\n") < 0 )
    {
        throw libc::exception(ferror(stdout),"fprintf to %s",YOCTO_NULL_DEVICE);
    }
}
YOCTO_UNIT_TEST_DONE()

