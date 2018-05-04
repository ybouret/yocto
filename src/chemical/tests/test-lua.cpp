
#include "yocto/chemical/lua.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/lua/lua-state.hpp"

using namespace yocto;
using namespace chemical;

YOCTO_UNIT_TEST_IMPL(lua)
{
    Lua::State vm;
    if(argc>1)
    {
        vm.DoFile( argv[1] );
    }

    for(int i=2;i<argc;++i)
    {
        vm.DoString(argv[i]);
    }

    lua_State *L = *vm;

    library lib("lib");
    __lua::load(L,lib);
    
}
YOCTO_UNIT_TEST_DONE()

