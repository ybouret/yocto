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

    ios::null_device & dev_null = ios::null_device::instance();
    fprintf(stderr,"name=%s\n", dev_null.name );
    dev_null.for_stdin();
    dev_null.for_stderr();
    dev_null.for_stdout();
    fprintf(stdout,"Hello, World!\n");
    fprintf(stderr,"World, Hello!\n");
}
YOCTO_UNIT_TEST_DONE()

YOCTO_UNIT_TEST_IMPL(null_device_cpp)
{
#if 0
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
#endif

    ios::null_device & dev_null = ios::null_device::instance();
    std::cerr << "name=" << dev_null.name << std::endl;
    dev_null.for_cin();
    dev_null.for_cout();
    dev_null.for_cerr();

    std::cout << "Hello, World!" << std::endl;
    std::cerr << "World, Hello!" << std::endl;

}
YOCTO_UNIT_TEST_DONE()

