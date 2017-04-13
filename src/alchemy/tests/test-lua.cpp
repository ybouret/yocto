#include "yocto/utest/run.hpp"
#include "yocto/alchemy/lua/interface.hpp"

using namespace yocto;
using namespace alchemy;


YOCTO_UNIT_TEST_IMPL(lua)
{
    
    library::pointer    pLib( new library() );
    Lua::State::Pointer vm( new Lua::State() );
    if(argc>1)
    {
        vm->DoFile(argv[1]);
        for(int i=2;i<argc;++i)
        {
            vm->DoString(argv[i]);
        }
    }

    __lua::load(vm, "species", *pLib);
    pLib->display(std::cerr);

    equilibria::pointer pEqs( new equilibria(pLib) );

    __lua::load(vm,"eqs",*pEqs);

    pEqs->compile();



    boot loader(pLib);
    __lua::load(vm, "sol", loader);




    vector<double> C0( pLib->size() + 2 );
    //pEqs->computeK(0.0);
    //pEqs->warm_up(C0);
    //pEqs->init0(C0);
    loader.solution(C0,*pEqs,0);
    
    if(pLib->search("H+"))
    {
        std::cerr << "pH=" << pLib->pH(C0) << std::endl;
    }

}
YOCTO_UNIT_TEST_DONE()


