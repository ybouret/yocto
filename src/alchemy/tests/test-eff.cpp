#include "yocto/alchemy/lua/interface.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/math/core/tao.hpp"

using namespace yocto;
using namespace alchemy;


YOCTO_UNIT_TEST_IMPL(eff)
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
    __lua::load(vm,"sol",loader);

    __lua::Effectors edb(vm,"eff");

    const char *v_name[] = { "zeta"  };
    const char *p_name[] = { "zeta0" };
    variables   params(chemlib,v_name,p_name,sizeof(v_name)/sizeof(v_name[0]));

    vector<double> Cin( params.count,0.0 );
    vector<double> Cout( params.count,0.0);

    vector<double> rho(params.count,0.0);

    loader.solution(Cin,chemsys);
    std::cerr << "Cin=" << Cin << std::endl;
    math::tao::set(Cout,Cin);
    chemsys->rescale(Cout,0.8);
    std::cerr << "Cout=" << Cout << std::endl;

    for( effectors::iterator i=edb.begin(); i != edb.end(); ++i)
    {
        effector &eff = **i;
        std::cerr << "rate of " << eff.name << std::endl;
        math::tao::ld(rho,0);
        eff.rate(rho, 0.0, Cin, Cout,params);
        std::cerr << "\trho=" << rho << std::endl;
    }




}
YOCTO_UNIT_TEST_DONE()
