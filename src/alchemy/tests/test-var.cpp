#include "yocto/alchemy/variables.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace alchemy;


YOCTO_UNIT_TEST_IMPL(var)
{
    library::pointer lib( new library() );
    lib->add("H+", 1);
    lib->add("HO-",1);

    const char *v_name[] =  { "zeta", "V" };
    const char *p_name[] =  { "zeta0", "V0" };

    variables sol1(lib,v_name,p_name,sizeof(v_name)/sizeof(v_name[0]));

    std::cerr << "sol1=" << sol1 << std::endl;

}
YOCTO_UNIT_TEST_DONE()
