#include "yocto/threading/scheme/executor.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;


YOCTO_UNIT_TEST_IMPL(schemes)
{
    threading::seq_executor seq;
    threading::par_executor par;
}
YOCTO_UNIT_TEST_DONE()
