#include "yocto/alchemy/boot.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/code/rand.hpp"

using namespace yocto;
using namespace alchemy;


YOCTO_UNIT_TEST_IMPL(boot)
{
    library::pointer chemlib( new library() );

    chemlib->add("H+",  1);
    chemlib->add("HO-",-1);

    chemlib->add("Na+", 1);
    chemlib->add("Cl-",-1);

    chemlib->add("AH",0);
    chemlib->add("A-",-1);

    chemlib->add("NH4+",1);
    chemlib->add("NH3",0);


    boot loader(chemlib);

    loader.electroneutrality();
    loader.osmolarity(0.042);
    loader.conserve("Na+",     0.01);
    loader.conserve("AH",  "A-",0.000);
    loader.conserve("NH4+","NH3",1e-5);

    std::cerr << loader << std::endl;
    equilibria chemsys(chemlib);
    
    {
        equilibrium &eq = chemsys.add("water",1e-14);
        eq.add("H+", 1);
        eq.add("HO-",1);
    }
    
    {
        equilibrium &eq = chemsys.add("base",1e-9);
        eq.add("NH3",1);
        eq.add("H+",1);
        eq.add("NH4+",-1);

    }

    {
        equilibrium &eq = chemsys.add("acid",1e-4);
        eq.add("H+",1);
        eq.add("A-",1);
        eq.add("AH",-1);
    }



    chemsys.compile();
    loader.run2(chemsys,0.0);
    //chemlib->display(std::cerr,chemsys.C);

}
YOCTO_UNIT_TEST_DONE()


