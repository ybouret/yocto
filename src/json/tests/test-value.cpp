#include "yocto/json/value.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;

YOCTO_UNIT_TEST_IMPL(value)
{
    std::cerr << "sizeof(JSON::Value)=" << sizeof(JSON::Value) << std::endl;
    JSON::Value v;
}
YOCTO_UNIT_TEST_DONE()

