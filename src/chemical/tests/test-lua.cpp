
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

    vector<double> C0(cs.M,0);
    for(size_t iter=1;iter<=100;++iter)
    {
        for(size_t j=cs.M;j>0;--j)
        {
            C0[j] = 1e-3 * (alea.get<double>()-0.3);
        }
        std::cerr << "initial: " << std::endl;
        lib.display(C0);
        if(!cs.balance(C0))
        {
            std::cerr << "unable to balance..." << std::endl;
            continue;
        }
        std::cerr << "balanced: " << std::endl;
        lib.display(C0);
        std::cerr << "normalizing..." << std::endl;
        cs.normalize(C0,0.0);
        break;
    }


}
YOCTO_UNIT_TEST_DONE()

