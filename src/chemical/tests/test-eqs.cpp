#include "yocto/utest/run.hpp"
#include "yocto/chemical/equilibrium.hpp"
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

    std::cerr << lib << std::endl;

    constant_equilibrium water("water",1.0e-14);
    water(H,1);
    water(OH,1);
    std::cerr << water << std::endl;

    constant_equilibrium acetic("acetic",pow(10, -4.7));
    acetic(AH,-1);
    acetic(Am,1);
    acetic(H,1);
    std::cerr << acetic << std::endl;

}
YOCTO_UNIT_TEST_DONE()


