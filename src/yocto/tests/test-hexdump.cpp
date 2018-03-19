#include "yocto/string/hexdump.hpp"
#include "yocto/ios/icstream.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/ios/file-loader.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;

YOCTO_UNIT_TEST_IMPL(hexdump)
{
    if(argc>1)
    {
        ios::file_content content;
        ios::file_loader::append_to(content,argv[1]);
        ios::wcstream fp("hexdump.dat");
        hexdump::write(fp,content);
    }
}
YOCTO_UNIT_TEST_DONE()

