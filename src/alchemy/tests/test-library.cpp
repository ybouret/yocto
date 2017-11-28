#include "yocto/alchemy/equilibrium.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/sequence/vector.hpp"

using namespace yocto;
using namespace alchemy;

#define __SHOW_SIZE(A) std::cerr << "sizeof(" #A ")=" << sizeof(A) << std::endl

YOCTO_UNIT_TEST_IMPL(library)
{

    __SHOW_SIZE(species);
    __SHOW_SIZE(library);
    __SHOW_SIZE(equilibrium);
    __SHOW_SIZE(actor);
    
    library::pointer chemlib( new library() );

    chemlib->add("H+",1);
    chemlib->add("CH3COO-",-1);

    chemlib->display(std::cerr);

    vector<double> C( chemlib->size() + 2);
    for(size_t i=1;i<=C.size(); ++i)
    {
        C[i] = alea.to<double>();
    }

    chemlib->display(std::cerr,C);
    std::cerr << "C=" << C << std::endl;
    
}
YOCTO_UNIT_TEST_DONE()
