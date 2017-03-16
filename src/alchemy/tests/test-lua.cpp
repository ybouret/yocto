#include "yocto/utest/run.hpp"
#include "yocto/alchemy/lua/interface.hpp"
#include "yocto/alchemy/eqtree.hpp"

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


    eqNode *tree = eqNode::build(*pEqs);
    if(tree)
    {
        delete tree;
    }

    vector<double> C0( pLib->size() + 2 );

    loader.initialize(C0,*pEqs);


}
YOCTO_UNIT_TEST_DONE()


