#include "yocto/ink/ops/blur.hpp"
#include "yocto/ink/ops/mapper.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/ink/image.hpp"

using namespace yocto;
using namespace Ink;

template <typename T>
static inline void do_blur(Pixmap<T>       &tgt,
                           const Pixmap<T> &src,
                           Blur            &blr,
                           Engine          &engine,
                           const string    &id)
{
    YOCTO_IMG_IO();
    blr.apply(tgt,src,engine);
    IMG.save(tgt, id, NULL);
}

YOCTO_UNIT_TEST_IMPL(blur)
{
    {
        ios::wcstream fp("blur.dat");
        for(float sig=0.01f;sig<=10;sig+=0.01f)
        {
            const Blur blur(sig);
            fp( "%g %u %g\n", blur.sigma, unsigned(blur.length), blur(blur.length)  );
        }
    }

    YOCTO_IMG();
    Engine::SharedServer parSrv( new threading::par_server() );
    Blur blurA(0.1f); std::cerr << "blurA.length=" << blurA.length << std::endl;
    Blur blurB(1.0f); std::cerr << "blurB.length=" << blurB.length << std::endl;
    Blur blurC(4.0f); std::cerr << "blurC.length=" << blurC.length << std::endl;

    
    if(argc>1)
    {
        const Pixmap<RGB>     img3( IMG.loadRGB(argv[1], NULL) );
        const Pixmap<float>   imgf( img3, Convert::RGB2F );
        const Pixmap<uint8_t> img1( img3, Convert::RGB2U );
        Engine                par(img3,parSrv);
        IMG.save(img3, "img3.png", NULL);
        IMG.save(imgf, "imgf.png", NULL);
        IMG.save(img1, "img1.png", NULL);

        Pixmap<RGB>     pxm3(img3.w,img3.h);
        Pixmap<float>   pxmf(img3.w,img3.h);
        Pixmap<uint8_t> pxm1(img3.w,img3.h);

        do_blur(pxm3,img3,blurA,par,"blurA3.png");
        do_blur(pxm3,img3,blurB,par,"blurB3.png");
        do_blur(pxm3,img3,blurC,par,"blurC3.png");

        do_blur(pxmf,imgf,blurA,par,"blurAf.png");
        do_blur(pxmf,imgf,blurB,par,"blurBf.png");
        do_blur(pxmf,imgf,blurC,par,"blurCf.png");

        do_blur(pxm1,img1,blurA,par,"blurAf.png");
        do_blur(pxm1,img1,blurB,par,"blurBf.png");
        do_blur(pxm1,img1,blurC,par,"blurCf.png");

    }

}
YOCTO_UNIT_TEST_DONE()

