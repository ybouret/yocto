

#include "yocto/utest/run.hpp"
#include "yocto/chemical/equilibria.hpp"
#include "yocto/chemical/library.hpp"

using namespace yocto;
using namespace chemical;

YOCTO_UNIT_TEST_IMPL(norm)
{
    library lib( "lib" );
    species &H   = lib.add("H+",1);
    species &OH  = lib.add("HO-",-1);
#if 1
    species &AH  = lib.add("AH",0);
    species &Am  = lib.add("Am",-1);
    species &NH4 = lib.add("NH4+",1);
    species &NH3 = lib.add("NH3",0);
    species &OxH2 = lib.add("OxH2",0);
    species &OxHm = lib.add("OxH-",-1);
    species &Oxmm = lib.add("Ox--",-2);
#endif
    lib.add("Na+",1);
    lib.add("Cl-",-1);

    std::cerr << lib << std::endl;
    equilibria   cs("eqs");
    if(true)
    {
        equilibrium &water = cs("water",1.0e-14);
        water(H,1);
        water(OH,1);
        std::cerr << water << std::endl;
    }

#if 1
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
    std::cerr << "active=" << cs.active << std::endl;
    const size_t   M = cs.M;
    vector<double> C0(M+2);
    std::cerr << cs << std::endl;
    std::cerr << "Nu=" << cs.Nu << std::endl;
    
    for(size_t iter=1;iter<=1000;++iter)
    {
        for(size_t i=1;i<=M;++i)
        {
            C0[i] = 10   * alea.to<double>();
            //C0[i] = 1e-6 * alea.to<double>();
            //C0[i] = 0;
        }
        //C0[1] = 0.1;
        //C0[2] = 0.1;
        lib.display(C0);
        std::cerr << "<normalizing>" << std::endl;
        cs.normalize(C0,0.0);
        std::cerr << "<normalizing/>" << std::endl;
        break;
    }
    std::cerr << "Nu=" << cs.Nu << std::endl;
    std::cerr << cs << std::endl;
}
YOCTO_UNIT_TEST_DONE()


