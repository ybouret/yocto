
#include "yocto/chemical/lua.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/lua/lua-state.hpp"

using namespace yocto;
using namespace chemical;

YOCTO_UNIT_TEST_IMPL(lua)
{
    Lua::State::Pointer vm( new Lua::State() );

    if(argc>1)
    {
        vm->DoFile( argv[1] );
    }

    for(int i=2;i<argc;++i)
    {
        vm->DoString(argv[i]);
    }

    
    library lib("species");
    __lua::load(vm,lib);
    std::cerr <<  lib.name << ":" << std::endl;
    std::cerr << lib << std::endl;

    equilibria cs("cs");
    __lua::load(vm,cs,lib);
    std::cerr << cs << std::endl;
}
YOCTO_UNIT_TEST_DONE()

