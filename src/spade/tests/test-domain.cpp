#include "yocto/spade/domain.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace spade;

YOCTO_UNIT_TEST_IMPL(domain)
{
    {
        const Layout1D L(1,30);
        DomainInfo<coord1D> D(L,0,2,NULL);
        std::cerr << "full=" << D.full << std::endl;
        std::cerr << "part=" << D.part << std::endl;

    }
}
YOCTO_UNIT_TEST_DONE()
