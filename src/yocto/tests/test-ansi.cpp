#include "yocto/utest/run.hpp"
#include "yocto/ios/null-device.hpp"
#include "yocto/exceptions.hpp"
#include <fstream>

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

YOCTO_UNIT_TEST_IMPL(null_device_cpp)
{
    std::ifstream f_in(YOCTO_NULL_DEVICE);
    if( f_in.fail() )
    {
        throw exception("ifstream(%s)",YOCTO_NULL_DEVICE);
    }


    std::ofstream f_out(YOCTO_NULL_DEVICE);
    if( f_out.fail() )
    {
        throw exception("ofstream(%s)",YOCTO_NULL_DEVICE);
    }
    f_out << "Hello, World!" << std::endl;

    std::streambuf *inbuf = std::cin.rdbuf(f_in.rdbuf());
    if(inbuf)
    {
        //delete inbuf;
    }

    std::streambuf *outbuf = std::cout.rdbuf(f_out.rdbuf());
    if(outbuf)
    {

    }
    std::cout << "World, Hello!" << std::endl;
    
}
YOCTO_UNIT_TEST_DONE()

