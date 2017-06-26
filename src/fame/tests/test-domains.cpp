#include "yocto/fame/domains.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace fame;

YOCTO_UNIT_TEST_IMPL(domains)
{

    {
        const layout<coord1d>  full(1,32);
        {
            const domains<coord1d> doms(4,NULL,full,1);
            for(size_t i=0;i<doms.size;++i)
            {
                const domain<coord1d> &dom = doms[i];
                std::cerr << "doms[" << i << "].full.rank=" << dom.self.rank << std::endl;
            }
        }

        {
            const domains<coord1d> doms(4,NULL,full,0);
        }
    }
}
YOCTO_UNIT_TEST_DONE()
