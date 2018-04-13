#include "yocto/utest/run.hpp"
#include "yocto/chemical/equilibria.hpp"
#include "yocto/chemical/library.hpp"

using namespace yocto;
using namespace chemical;

YOCTO_UNIT_TEST_IMPL(eqs)
{
    library lib( "lib" );
    species &H   = lib.add("H+",1);
    species &OH  = lib.add("HO-",-1);
    species &AH  = lib.add("AH",0);
    species &Am  = lib.add("Am",-1);
    lib.add("Na+",1);
    lib.add("Cl-",-1);

    std::cerr << lib << std::endl;
    equilibria   cs("eqs");
    equilibrium &water = cs("water",1.0e-14);
    water(H,1);
    water(OH,1);
    std::cerr << water << std::endl;

    equilibrium &acetic = cs("acetic",pow(10, -4.7));
    acetic(AH,-1);
    acetic(Am,1);
    acetic(H,1);
    std::cerr << acetic << std::endl;

    std::cerr << cs << std::endl;

    cs.compile_for(lib);
    for(size_t j=cs.M;j>0;--j)
    {
        cs.C[j] = alea.to<double>();
    }
    std::cerr << "N="   << cs.N << std::endl;
    std::cerr << "M="   << cs.M << std::endl;
    std::cerr << "nu="  << cs.nu << std::endl;
    std::cerr << "nuT=" << cs.nuT << std::endl;
    std::cerr << "active=" << cs.active << std::endl;

    cs.initializeGamma(0.0);
    std::cerr << "C=" << cs.C << std::endl;
    std::cerr << "Gam=" << cs.Gam << std::endl;
    lib.display(cs.C);
}
YOCTO_UNIT_TEST_DONE()


