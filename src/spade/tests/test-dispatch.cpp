#include "yocto/spade/split.hpp"
#include "yocto/utest/run.hpp"
#include <cstdlib>

using namespace yocto;
using namespace spade;

YOCTO_UNIT_TEST_IMPL(dispatch)
{
    if(argc>1)
    {
        for(int i=1;i<argc;++i)
        {
            const size_t cpus = max_of<size_t>(1, atol(argv[i]) );
            std::cerr << "splitting #cpus=" << cpus << std::endl;
            
            {
                const Layout2D Lsq( coord2D(1,1), coord2D(256,256) );
                const coord2D  nsq = Split::DispatchCPUs(Lsq,cpus);
                std::cerr <<  "nsq=" << nsq << std::endl;
            }
            
            {
                const Layout3D Lsq( coord3D(1,1,1), coord3D(256,256,256) );
                const coord3D  nsq = Split::DispatchCPUs(Lsq,cpus);
                std::cerr <<  "nsq=" << nsq << std::endl;
            }
        }
        
    }
}
YOCTO_UNIT_TEST_DONE()
