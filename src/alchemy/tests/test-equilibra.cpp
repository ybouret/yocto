#include "yocto/alchemy/equilibria.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/code/rand.hpp"

using namespace yocto;
using namespace alchemy;


YOCTO_UNIT_TEST_IMPL(equilibria)
{
    
    
    
    library::pointer chemlib( new library() );
    
    chemlib->add("H+",1);
    chemlib->add("HO-",-1);

    chemlib->add("Na+",1);
    chemlib->add("Cl-",-1);

    chemlib->add("AH",0);
    chemlib->add("A-",-1);

    chemlib->add("NH4+",1);
    chemlib->add("NH3",0);

    vector<double> C( chemlib->size() + 2);
    for(size_t i=1;i<=C.size(); ++i)
    {
        C[i] = 1e-4*alea<double>()*(0.8-alea<double>());
    }



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
    std::cerr << chemsys << std::endl;
    std::cerr << "Nu     = "  << chemsys.Nu     << std::endl;
    std::cerr << "iNu    = "  << chemsys.iNu    << std::endl;
    std::cerr << "NuT    = "  << chemsys.NuT    << std::endl;

    std::cerr << "active = "  << chemsys.active << std::endl;
    for(size_t i=1;i<=chemlib->size();++i)
    {
        chemsys.C[i] = C[i];
    }
    chemlib->display(std::cerr,chemsys.C);

    chemsys.balance();
    

    return 0;

#if 0
    chemsys.balance();

    
    chemsys.computeChi(chemsys.C, 0);
    std::cerr << "Gamma  = " << chemsys.Gamma << std::endl;
    std::cerr << "Phi    = " << chemsys.Phi   << std::endl;

    std::cerr << "-- Normalizing..." << std::endl;
    chemsys.normalize(chemsys.C, 0.0);
#endif

}
YOCTO_UNIT_TEST_DONE()
