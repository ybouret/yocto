#include "yocto/lua/lua-state.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;


YOCTO_UNIT_TEST_IMPL(table)
{
    Lua::State vm;
    lua_State *L = *vm;

    lua_newtable(L);
    vm.SetField<double>("a",1.0);
    vm.SetField<string>("b","hello");
    vm.SetEntry<int>(5,4);
    assert(lua_istable(L,-1));
    lua_setglobal(L,"toto");
    vm.DoString("for k in pairs(toto) do print( \"toto[\" .. k .. \"]=\" .. toto[k] ) end");

    lua_settop(L,0);
    lua_getglobal(L,"toto");
    assert(lua_istable(L,-1));
    const size_t n = vm.GetTableLength();
    assert(lua_istable(L,-1));
    std::cerr << "#=" << n << std::endl;




}
YOCTO_UNIT_TEST_DONE()
