
#include "yocto/chemical/lua.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/lua/lua-state.hpp"
#include "yocto/ios/ocstream.hpp"

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
    {
        boot loader("ini");
        __lua::load(vm,loader,lib);
        std::cerr << loader << std::endl;



        loader.guess(C0,cs,0.0);
        lib.display(C0);
    }

    if( vm->Has("add") )
    {
        std::cerr << "Start titration..." << std::endl;
        const bool has_proton = lib.search("H+") != NULL;
        if(has_proton)
        {
            std::cerr << "pH0=" << lib.pH(C0) << std::endl;
        }
        vector<double> C1(cs.M,0);
        {
            boot loader("add");
            __lua::load(vm,loader,lib);
            std::cerr << loader << std::endl;
            loader.guess(C1,cs,0);
        }

        vector<double> Cmix(cs.M,0);
        const double V0 = 10;
        const size_t Nd = 1000;
        ios::wcstream fp("dosage.dat");
        for(size_t i=0;i<=Nd;++i)
        {
            const double V1 = (2*V0*i)/Nd;
            const int iv =int(floor(V1*100+0.5));
            if(  iv == 634 || iv == 632 )
            {
                std::cerr << std::endl;
                std::cerr << "V1=" << V1 << std::endl;
                //cs.updateGammaAndPhi(cs.C);
                //cs.compute_step();
            }
            if(!cs.mix(Cmix,C0,V0,C1,V1,0.0))
            {
                throw exception("cannot mix @V1=%g", V1);
            }
            std::cerr << ".";
            fp("%g",V1);
            if(634==iv) exit(0);

            if(has_proton)
            {
                fp(" %g", lib.pH(Cmix) );
            }
            for(size_t j=1;j<=cs.M;++j)
            {
                fp(" %g", Cmix[j]);
            }
            fp("\n");
        }
        std::cerr << std::endl;

    }



}
YOCTO_UNIT_TEST_DONE()

