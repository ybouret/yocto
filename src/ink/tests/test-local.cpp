
#include "yocto/ink/ops/local.hpp"
#include "yocto/ink/image.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace Ink;

YOCTO_UNIT_TEST_IMPL(local)
{
    YOCTO_IMG();
    Engine::SharedServer seqSrv( new threading::seq_server() );
    Engine::SharedServer parSrv( new threading::par_server() );

    for(size_t i=0;i<Local::SIZE;++i)
    {
        const coord &C    = Local::Position[i]; std::cerr << "Position[" << i << "]=" << C    << std::endl;
        const size_t flag = Local::Flags[i];    std::cerr << "Flag    [" << i << "]=" << flag << std::endl;
        const size_t ipos = Local::GetPositionIndex(flag); std::cerr << "---> pos=" << ipos << std::endl;
        std::cerr << std::endl;
        if(ipos!=i) throw exception("Local Internal Corruption");
    }

    if(argc>1)
    {
        const PixmapRGB img( IMG.loadRGB(argv[1],NULL) );
        IMG.save(img,"img.png",NULL);
        Engine          par(img,parSrv);
        Engine          seq(img,seqSrv);

    }
}
YOCTO_UNIT_TEST_DONE()

