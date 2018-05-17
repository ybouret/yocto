
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
    std::cerr << lib.name << ":" << std::endl;
    std::cerr << lib << std::endl;

    equilibria cs("cs");
    __lua::load(vm,cs,lib);
    std::cerr << cs << std::endl;

    boot loader("ini");
    __lua::load(vm,loader,lib);
    std::cerr << loader << std::endl;


    vector<double> C0(cs.M,0);

    loader.guess(C0,cs,0.0);
    lib.display(C0);
    

}
YOCTO_UNIT_TEST_DONE()

