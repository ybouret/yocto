#include "yocto/alchemy/boot.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/code/rand.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/ios/ocstream.hpp"

using namespace yocto;
using namespace alchemy;
using namespace math;

YOCTO_UNIT_TEST_IMPL(titration)
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
    const double C0 = 1e-4;
    std::cerr << "Computing Acid Composition" << std::endl;
    vector<double> Cacid(chemsys.M);
    {
        boot loader(chemlib);
        loader.electroneutrality();
        loader.conserve("AH", "A-",    C0);
        loader.conserve("NH3", "NH4+", 0);
        loader.conserve("Na+", 0);
        loader.conserve("Cl-", 0);

        //loader.initialize(Cacid,chemsys,0.0);
        //tao::set(Cacid,loader.run(chemsys,0.0));
        loader.solution(Cacid,chemsys);
        std::cerr << "Cacid=" << std::endl;
        chemlib->display(std::cerr,Cacid);
        std::cerr << "pH=" << chemlib->pH(Cacid) << std::endl;
    }

    std::cerr << "Computing Basic Composition" << std::endl;
    vector<double> Csoda(chemsys.M);
    {
        boot loader(chemlib);
        loader.electroneutrality();
        loader.conserve("AH", "A-",    0);
        loader.conserve("NH3", "NH4+", 0);
        loader.conserve("Na+", C0);
        loader.conserve("Cl-", 0);

        loader.solution(Csoda,chemsys);
        std::cerr << "Csoda=" << std::endl;
        chemlib->display(std::cerr,Csoda);
        std::cerr << "pH=" << chemlib->pH(Csoda) << std::endl;
    }
    
    vector<double> CC(chemsys.M);
    
    std::cerr << "Mixing..." << std::endl;
    {
        ios::wcstream fp("acid.dat");
        const double Vacid = 10;
        for(double Vsoda=0;Vsoda<=2*Vacid;Vsoda+=0.01)
        {
            const double Vtot = Vacid + Vsoda;
            chemsys.mix(CC,Vacid/Vtot,Cacid,Vsoda/Vtot,Csoda);
            fp("%g %g\n", Vsoda, chemlib->pH(CC));
        }
    }




}
YOCTO_UNIT_TEST_DONE()

