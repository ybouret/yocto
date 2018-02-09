
#include "yocto/ink/ops/local.hpp"
#include "yocto/ink/ops/mapper.hpp"

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

    Filter filter;

    if(argc>1)
    {
        const PixmapRGB img( IMG.loadRGB(argv[1],NULL) );
        const PixmapF   imgF( img, Convert::RGB2F);
        IMG.save(img, "img.png", NULL);
        IMG.save(imgF,"imgf.png",NULL);

        Engine          par(img,parSrv);
        Engine          seq(img,seqSrv);

        PixmapRGB tgt1(img.w,img.h);
        PixmapF   tgt1F(img.w,img.h);

        filter.run(tgt1,img,Filter::Erode<RGB>,par);
        IMG.save(tgt1,"erode.png",NULL);

        filter.run(tgt1F,imgF,Filter::Erode<float>,par);
        IMG.save(tgt1F,"erodef.png",NULL);

        filter.run(tgt1,img,Filter::Dilate<RGB>,par);
        IMG.save(tgt1,"dilate.png",NULL);

        filter.run(tgt1F,imgF,Filter::Dilate<float>,par);
        IMG.save(tgt1F,"dilatef.png",NULL);


        filter.run(tgt1,img,Filter::Average<RGB>,par);
        IMG.save(tgt1,"average.png",NULL);

        filter.run(tgt1F,imgF,Filter::Average<float>,par);
        IMG.save(tgt1F,"averagef.png",NULL);
    }
}
YOCTO_UNIT_TEST_DONE()

