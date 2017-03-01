#include "yocto/alchemy/boot.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/code/rand.hpp"

using namespace yocto;
using namespace alchemy;


static inline double Kv(const double t)
{
    return 1e-4*(2.0 + cos(t));
}

YOCTO_UNIT_TEST_IMPL(ode)
{



    library::pointer chemlib( new library() );

    chemlib->add("H+",1);
    chemlib->add("HO-",-1);


    chemlib->add("Na+", 1);
    chemlib->add("Cl-",-1);

    chemlib->add("AH",0);
    chemlib->add("A-",-1);

    equilibria::pointer chemsys( new equilibria(chemlib) );


    {
        equilibrium &eq = chemsys->add("water",1e-14);
        eq.add("H+", 1);
        eq.add("HO-",1);
    }



    {
        equilibrium_constant Kval = cfunctor(Kv);
        equilibrium &eq = chemsys->add("acid",Kval);
        eq.add("H+",1);
        eq.add("A-",1);
        eq.add("AH",-1);
    }

    chemsys->compile();
    
    boot loader(chemlib);


    loader.electroneutrality();
    loader.conserve("Na+", 0);
    loader.conserve("Cl-", 0);
    loader.conserve("AH", "A-", 1e-4);

    loader.run2(*chemsys, 0.0);




}
YOCTO_UNIT_TEST_DONE()

