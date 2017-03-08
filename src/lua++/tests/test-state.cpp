#include "yocto/lua/lua-state.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;


YOCTO_UNIT_TEST_IMPL(state)
{
    Lua::State L;
    L.DumpStack();
}
YOCTO_UNIT_TEST_DONE()
