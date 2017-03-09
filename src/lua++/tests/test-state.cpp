#include "yocto/lua/lua-state.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;


YOCTO_UNIT_TEST_IMPL(state)
{
    Lua::State L;
    L.DumpStack();

    for(int i=1;i<argc;++i)
    {
        std::cerr << "executing '" << argv[i] << "'" << std::endl;
        L.DoString(argv[i]);
    }

}
YOCTO_UNIT_TEST_DONE()
