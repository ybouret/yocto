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
    std::cerr << "Apply Blur(" << blr.length << ")\ton a " << tgt.w << "x" << tgt.h << "x" << tgt.depth << " pixmap" << std::endl;
    YOCTO_IMG_IO();
    Pixmap<T> tmp(tgt.w,tgt.h);
    blr.apply(tgt,tmp,src,engine);
    IMG.save(tgt, id, NULL);
}

YOCTO_UNIT_TEST_IMPL(blur)
{
    if(true)
    {
        ios::wcstream fp("blur.dat");
        for(float sig=0.01f;sig<=10;sig+=0.01f)
        {
            const Blur blur(sig);
            fp( "%g %u %g\n", blur.sigma, unsigned(blur.length), blur( float(blur.length) )  );
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

        do_blur(pxm1,img1,blurA,par,"blurA1.png");
        do_blur(pxm1,img1,blurB,par,"blurB1.png");
        do_blur(pxm1,img1,blurC,par,"blurC1.png");

    }

}
YOCTO_UNIT_TEST_DONE()

