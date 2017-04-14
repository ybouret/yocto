#include "yocto/utest/run.hpp"
#include "yocto/alchemy/lua/interface.hpp"

using namespace yocto;
using namespace alchemy;


YOCTO_UNIT_TEST_IMPL(lua)
{
    
    Lua::State::Pointer vm( new Lua::State() );
    if(argc>1)
    {
        vm->DoFile(argv[1]);
        for(int i=2;i<argc;++i)
        {
            vm->DoString(argv[i]);
        }
    }

    library::pointer chemlib( new __lua::Library(vm,"species") );
    chemlib->display(std::cerr);


    equilibria::pointer chemsys( new __lua::Equilibria(vm,"eqs",chemlib) );
    chemsys->compile();


    boot loader(chemlib);
    __lua::load(vm,"ini",loader);



    vector<double> C0( chemlib->size() + 2 );
    loader.solution(C0,chemsys,0);
    
    if(chemlib->search("H+"))
    {
        std::cerr << "pH=" << chemlib->pH(C0) << std::endl;
    }

}
YOCTO_UNIT_TEST_DONE()


