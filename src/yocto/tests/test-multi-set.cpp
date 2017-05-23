#include "yocto/associative/multi-set.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/string.hpp"

using namespace yocto;

YOCTO_UNIT_TEST_IMPL(multi_set)
{
    multi_set<string,double> ms;

    ms.insert("hello",1.0);
    ms.insert("hello",2.0);
    
}
YOCTO_UNIT_TEST_DONE()
