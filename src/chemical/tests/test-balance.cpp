
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

    if(false)
    {
        equilibrium &ammoniac = cs("ammoniac",pow(10,-9.8));
        ammoniac(NH4,-1);
        ammoniac(NH3,1);
        ammoniac(H,1);
    }

    cs.compile_for(lib);
    std::cerr << "active=" << cs.active << std::endl;
    const size_t   M = cs.M;
    vector<double> C0(M+2);
    for(size_t i=1;i<=M;++i)
    {
        C0[i] = 1e-3 * alea.symm<double>();
    }
    lib.display(C0);
    std::cerr << "nu2=" << cs.nu2 << std::endl;
    cs.balance(C0,0.0);
    lib.display(C0);

}
YOCTO_UNIT_TEST_DONE()


