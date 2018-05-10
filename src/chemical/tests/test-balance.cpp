
#include "yocto/utest/run.hpp"
#include "yocto/chemical/equilibria.hpp"
#include "yocto/chemical/library.hpp"

using namespace yocto;
using namespace chemical;

YOCTO_UNIT_TEST_IMPL(balance)
{
    library lib( "lib" );
    species &H   = lib.add("H+",1);
    species &OH  = lib.add("HO-",-1);
    species &AH  = lib.add("AH",0);
    species &Am  = lib.add("Am",-1);
    species &NH4 = lib.add("NH4+",1);
    species &NH3 = lib.add("NH3",0);
    lib.add("Na+",1);
    lib.add("Cl-",-1);
    lib.compile();
    
    std::cerr << lib << std::endl;
    equilibria   cs("eqs");
    {
        equilibrium &water = cs("water",1.0e-14);
        water(H,1);
        water(OH,1);
        std::cerr << water << std::endl;
    }

    if(true)
    {
        equilibrium &acetic = cs("acetic",pow(10, -4.7));
        acetic(AH,-1);
        acetic(Am,1);
        acetic(H,1);
        std::cerr << acetic << std::endl;
    }

    if(true)
    {
        equilibrium &ammoniac = cs("ammoniac",(pow(10,-9.8*2)));
        ammoniac(NH4,-2);
        ammoniac(NH3,2);
        ammoniac(H,2);
        std::cerr << ammoniac << std::endl;
    }

    std::cerr << cs << std::endl;
    cs.compile_for(lib);
    std::cerr << "active=" << cs.active << std::endl;
    const size_t   M = cs.M;

    std::cerr << "Nu=" << cs.Nu << std::endl;
    std::cerr << "NuT=" << cs.NuT << std::endl;

    vector<double> C0(M+2);

    for(size_t iter=1;iter<=1000;++iter)
    {
        std::cerr << "<balancing>" << std::endl;
        for(size_t i=1;i<=M;++i)
        {
            C0[i] = 1e-3 * (alea.get<double>()-0.3);
        }
        lib.display(C0);

        if(cs.balance(C0))
        {
            std::cerr << "balanced:" << std::endl;
            lib.display(C0);
        }
        else
        {
            std::cerr << "unable to balance" << std::endl;
        }
        std::cerr << "<balancing/>" << std::endl << std::endl;
        //std::cerr << "break..." << std::endl; break;
    }
    std::cerr << "Nu=" << cs.Nu << std::endl;
    
}
YOCTO_UNIT_TEST_DONE()

YOCTO_UNIT_TEST_IMPL(balance2)
{
    library lib( "lib" );
    species &H   = lib.add("H+",1);
    species &OH  = lib.add("HO-",-1);
    species &AH  = lib.add("AH",0);
    species &Am  = lib.add("Am",-1);
    lib.compile();

    std::cerr << lib << std::endl;
    equilibria   cs("eqs");
    {
        equilibrium &water = cs("water",1.0e-14);
        water(H,1);
        water(OH,1);
        std::cerr << water << std::endl;
    }

    if(true)
    {
        equilibrium &acetic = cs("acetic",pow(10, -4.7));
        acetic(AH,-1);
        acetic(Am,1);
        acetic(H,1);
        std::cerr << acetic << std::endl;
    }

    cs.compile_for(lib);

    vector<double> C0(cs.M,0);

    C0[3] = -0.1;
    C0[4] = 0.2;
    lib.display(C0);
    if(cs.balance(C0))
    {
        std::cerr << "balanced" << std::endl;
        lib.display(C0);
    }


}
YOCTO_UNIT_TEST_DONE()

