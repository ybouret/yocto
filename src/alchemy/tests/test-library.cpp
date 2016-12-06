#include "yocto/alchemy/library.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/code/rand.hpp"

using namespace yocto;
using namespace alchemy;

YOCTO_UNIT_TEST_IMPL(library)
{
    library::pointer chemlib( new library() );

    chemlib->add("H+",1);
    chemlib->add("CH3COO-",-1);

    chemlib->display(std::cerr);

    vector<double> C( chemlib->size() + 2);
    for(size_t i=1;i<=C.size(); ++i)
    {
        C[i] = alea<double>();
    }

    chemlib->display(std::cerr,C);
    std::cerr << "C=" << C << std::endl;

}
YOCTO_UNIT_TEST_DONE()
