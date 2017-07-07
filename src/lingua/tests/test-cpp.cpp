#include "yocto/lingua/prv/xgen-cpp.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace lingua;

YOCTO_UNIT_TEST_IMPL(cpp)
{
    xgen_cpp cpp;
    if(argc>1)
    {
        token content;
        cpp.process(argv[1],content);
        std::cerr << content << std::endl;
    }
}
YOCTO_UNIT_TEST_DONE()
