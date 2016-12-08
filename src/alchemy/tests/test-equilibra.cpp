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
    chemlib->add("AH",0);
    chemlib->add("A-",-1);
    
    vector<double> C( chemlib->size() + 2);
    for(size_t i=1;i<=C.size(); ++i)
    {
        C[i] = alea<double>();
    }
    
    equilibria chemsys(chemlib);
    
    {
        equilibrium &eq = chemsys.add("water",1e-14);
        eq.add("H+", 1);
        eq.add("HO-",1);
    }

    {
        equilibrium &eq = chemsys.add("acide",1e-4);
        eq.add("H+",1);
        eq.add("A-",1);
        eq.add("AH",-1);
    }

    
    chemsys.compile();
    std::cerr << "Nu    = "  << chemsys.Nu   << std::endl;
    std::cerr << "NuT   = "  << chemsys.NuT  << std::endl;

    chemsys.computeXi(C, 0);
    std::cerr << "Gamma = " << chemsys.Gamma << std::endl;
    std::cerr << "Phi   = " << chemsys.Phi   << std::endl;

    std::cerr << "-- Normalizing..." << std::endl;
    chemsys.normalize(C, 0.0);


}
YOCTO_UNIT_TEST_DONE()
