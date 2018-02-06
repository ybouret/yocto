#include "yocto/ink/ops/differential.hpp"
#include "yocto/ink/image.hpp"
#include "yocto/utest/run.hpp"



using namespace yocto;
using namespace Ink;

YOCTO_UNIT_TEST_IMPL(diff)
{
    YOCTO_IMG();
    std::cerr << "sizeof(Domain)=" << sizeof(Domain) << std::endl;
    std::cerr << "sizeof(cslot) =" << sizeof(cslot)  << std::endl;
    if(argc>1)
    {
        const PixmapRGB pxm3( IMG.loadRGB(argv[1],NULL) );
        
    }
    
}
YOCTO_UNIT_TEST_DONE()


