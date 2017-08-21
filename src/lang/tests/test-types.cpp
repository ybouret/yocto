#include "yocto/lang/char.hpp"
#include "yocto/utest/run.hpp"
using namespace yocto;
using namespace Lang;

#define __SHOW(TYPE) do { \
std::cerr << "sizeof( " #TYPE " )=" << sizeof(TYPE) << std::endl;\
} while(false)

YOCTO_UNIT_TEST_IMPL(types)
{
    __SHOW(CharInfo);
    __SHOW(Char);
}
YOCTO_UNIT_TEST_DONE()