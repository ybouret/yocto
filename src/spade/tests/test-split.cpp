#include "yocto/spade/split.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace spade;

YOCTO_UNIT_TEST_IMPL(split)
{
    {
        Layout1D full(1,100);
        std::cerr << "full=" << full << std::endl;
        for(size_t np=1;np<=8;++np)
        {
            std::cerr << "NP=" << np << std::endl;
            const Split::In1D in1D(full,np);
            for(size_t r=0;r<np;++r)
            {
                const Layout1D sub = in1D(r);
                std::cerr << "\tsub#" << r << "=" << sub << std::endl;
            }
        }
    }
}
YOCTO_UNIT_TEST_DONE()
