#include "yocto/lua/lua-state.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;


YOCTO_UNIT_TEST_IMPL(table)
{
    Lua::State vm;
    lua_State *L = *vm;

    lua_newtable(L);
    lua_setglobal(L,"toto");
    lua_getglobal(L,"toto");
    vm.SetField<double>("a",1.0);
    vm.SetField<string>("b","hello");
    vm.DoString("for k in pairs(toto) do print( \"toto[\" .. k .. \"]=\" .. toto[k] ) end");
    



}
YOCTO_UNIT_TEST_DONE()
