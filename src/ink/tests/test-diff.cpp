#include "yocto/ink/ops/differential.hpp"
#include "yocto/ink/color/conv.hpp"
#include "yocto/ink/image.hpp"
#include "yocto/utest/run.hpp"



using namespace yocto;
using namespace Ink;

YOCTO_UNIT_TEST_IMPL(diff)
{
    YOCTO_IMG();
    std::cerr << "sizeof(Domain)=" << sizeof(Domain) << std::endl;
    std::cerr << "sizeof(cslot) =" << sizeof(cslot)  << std::endl;

    Engine::SharedServer seqSrv( new threading::seq_server() );
    Engine::SharedServer parSrv( new threading::par_server() );

    if(argc>1)
    {
        const PixmapRGB pxm3( IMG.loadRGB(argv[1],NULL) );
        Engine          par(pxm3,parSrv);
        Engine          seq(pxm3,seqSrv);

        Differential    diff(pxm3);

        diff.apply(pxm3,Convert::Copy<float,float>,par);
    }
    
}
YOCTO_UNIT_TEST_DONE()


