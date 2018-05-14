
#include "yocto/utest/run.hpp"
#include "yocto/chemical/boot.hpp"
#include "yocto/chemical/library.hpp"

using namespace yocto;
using namespace chemical;

YOCTO_UNIT_TEST_IMPL(boot)
{
    library lib( "lib" );
    species &H   = lib.add("H+",1);
    species &OH  = lib.add("HO-",-1);
#if 0
    species &AH  = lib.add("AH",0);
    species &Am  = lib.add("Am",-1);
    species &NH4 = lib.add("NH4+",1);
    species &NH3 = lib.add("NH3",0);
    species &OxH2 = lib.add("OxH2",0);
    species &OxHm = lib.add("OxH-",-1);
    species &Oxmm = lib.add("Ox--",-2);
#endif
    species & Na = lib.add("Na+",1);
    species & Cl = lib.add("Cl-",-1);

    lib.compile();
    std::cerr << lib << std::endl;
    equilibria   cs("eqs");
    if(true)
    {
        equilibrium &water = cs("water",1.0e-14);
        water(H,1);
        water(OH,1);
        std::cerr << water << std::endl;
    }

#if 0
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
        equilibrium &ammoniac = cs("ammoniac",pow(10,-9.8));
        ammoniac(NH4,-1);
        ammoniac(NH3,1);
        ammoniac(H,1);
    }

    if(true)
    {
        {
            equilibrium &ox1 = cs("oxalic1",pow(10,-1.27));
            ox1(OxH2,-1);
            ox1(OxHm,1);
            ox1(H,1);
        }

        {
            equilibrium &ox2 = cs("oxalic2",pow(10,-4.27));
            ox2(OxHm,-1);
            ox2(Oxmm,1);
            ox2(H,1);
        }
    }
#endif

    cs.compile_for(lib);
    std::cerr << cs << std::endl;

    boot loader("ini");
    loader.electroneutrality(lib);

    {
        boot::constraint &c = loader.create(0.0);
        c.add(Na,1);
        c.add(Cl,1);
    }

    {
        boot::constraint &c = loader.create(0.0);
        c.add(Cl,1);
    }

    std::cerr << "loader " << loader.name << ":" << std::endl << loader << std::endl;
    vector<double> C0(cs.M);
    loader.guess(C0,cs,0.0);

}
YOCTO_UNIT_TEST_DONE()


