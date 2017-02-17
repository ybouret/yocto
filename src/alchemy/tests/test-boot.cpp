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
    loader.osmolarity(0.1);
    loader.conserve("Na+",0.01);
    loader.conserve("AH","A-",0.02);

    std::cerr << loader << std::endl;

}
YOCTO_UNIT_TEST_DONE()


