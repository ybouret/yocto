#include "yocto/alchemy/library.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace alchemy;

YOCTO_UNIT_TEST_IMPL(library)
{
    library chemlib;

    chemlib.add("H+",1);
    chemlib.add("CH3COO-",-1);

    chemlib.display(std::cerr);

}
YOCTO_UNIT_TEST_DONE()
