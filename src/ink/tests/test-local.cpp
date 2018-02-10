
#include "yocto/ink/ops/filter.hpp"
#include "yocto/ink/ops/mapper.hpp"

#include "yocto/ink/image.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace Ink;

template <typename T>
static inline void do_filter(AutoFilter<T>   &F,
                             const Pixmap<T> &src,
                             const char      *txt,
                             Engine          &engine)
{
    string ID = "op";
    ID += txt;
    YOCTO_IMG_IO();
    std::cerr << "Filtering on  " << ID << std::endl;
    IMG.save(src, ID + "_img.png",NULL);
    Pixmap<T> tgt(F.w,F.h);

#define _DO(OPS) do { \
std::cerr << "\t" #OPS << "..." << std::endl;\
tgt.copy(src); \
F.OPS(tgt,engine);\
IMG.save(tgt, ID + "_" #OPS ".png",NULL);\
} while(false)

    _DO(Erode);
    _DO(Dilate);
    _DO(Average);
    _DO(Median);
    _DO(Open);
    _DO(Close);
    std::cerr << std::endl;
}

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
        const PixmapRGB imgC( IMG.loadRGB(argv[1],NULL) );
        const PixmapF   imgF( imgC, Convert::RGB2F);
        const PixmapU   imgU( imgC, Convert::RGB2U);
        Engine          par(imgC,parSrv);
        Engine          seq(imgC,seqSrv);

        AutoFilter<RGB>     FC(imgC);
        AutoFilter<float>   FF(imgC);
        AutoFilter<uint8_t> FU(imgC);

        do_filter(FC,imgC, "3", par);
        do_filter(FF,imgF, "f", par);
        do_filter(FU,imgU, "u", par);


    }
}
YOCTO_UNIT_TEST_DONE()

