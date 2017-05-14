#include "yocto/spade/split.hpp"
#include "yocto/utest/run.hpp"
#include <cstdlib>

using namespace yocto;
using namespace spade;

YOCTO_UNIT_TEST_IMPL(dispatch)
{
    if(argc>1)
    {
        const size_t cpus = max_of<size_t>(1, atol(argv[1]) );
        std::cerr << "splitting #cpus=" << cpus << std::endl;

        const Layout2D Lsq( coord2D(1,1), coord2D(256,256) );
        const coord2D  nsq = Split::DispatchCPUs(Lsq,cpus);
        std::cerr <<  "nsq=" << nsq << std::endl;

    }
}
YOCTO_UNIT_TEST_DONE()
