
#include "yocto/ink/draw/line.hpp"
#include "yocto/ink/color/conv.hpp"
#include "yocto/ink/image.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/ink/color/named.hpp"

using namespace yocto;
using namespace Ink;


YOCTO_UNIT_TEST_IMPL(draw)
{
    YOCTO_IMG();

    const unit_t w = 200 + alea.leq(200);
    const unit_t h = 200 + alea.leq(200);

    const unit_t x0 = -w/2;
    const size_t w0 = 2*w;
    const unit_t y0 = -h/2;
    const unit_t h0 = 2*h;

    PixmapRGB    img3(w,h);
    PixmapF      imgf(w,h);
    PixmapU      img1(w,h);
    



    for(size_t k=0;k<1000;++k)
    {
        const unit_t  X = x0 + alea.leq(w0);
        const unit_t  Y = y0 + alea.leq(h0);
        const size_t  W = alea.leq(w0);
        const RGB     C = NamedColor::FetchRGB(k);
        const float   F = Convert::RGB2F(C);
        const uint8_t U = Convert::RGB2U(C);
        if( alea.get<float>() > 0.5f )
        {
            Draw::HLine(img3,X,Y,W,C);
            Draw::HLine(imgf,X,Y,W,F);
            Draw::HLine(img1,X,Y,W,U);
        }
        else
        {
            const uint8_t alpha = alea.leq(255);
            Draw::HLine(img3,X,Y,W,C,alpha);
            Draw::HLine(imgf,X,Y,W,F,alpha);
            Draw::HLine(img1,X,Y,W,U,alpha);
        }
    }

    IMG.save(img3, "img3.png", NULL);
    IMG.save(imgf, "imgf.png", NULL);
    IMG.save(img1, "img1.png", NULL);

}
YOCTO_UNIT_TEST_DONE()

