#include "yocto/utest/run.hpp"
#include "yocto/alchemy/lua/interface.hpp"

using namespace yocto;
using namespace alchemy;


YOCTO_UNIT_TEST_IMPL(lua)
{

    library::pointer pLib( new library() );
    Lua::State       vm;
    if(argc>1)
    {
        vm.DoFile(argv[1]);
        for(int i=2;i<argc;++i)
        {
            vm.DoString(argv[i]);
        }
    }

    __lua::load(vm, "species", *pLib);
    
}
YOCTO_UNIT_TEST_DONE()


