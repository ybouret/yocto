
#include "yocto/ink/draw/line.hpp"
#include "yocto/ink/draw/circle.hpp"
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

    const unit_t w = 400 + alea.leq(400);
    const unit_t h = 400 + alea.leq(400);

    const unit_t x0 = -w/2;
    const size_t w0 = 2*w;
    const unit_t y0 = -h/2;
    const unit_t h0 = 2*h;

    PixmapRGB    img3(w,h);
    PixmapF      imgf(w,h);
    PixmapU      img1(w,h);
    



    for(size_t k=0;k<100;++k)
    {
        const unit_t  X = x0 + alea.leq(w0);
        const unit_t  Y = y0 + alea.leq(h0);
        const size_t  W = alea.leq(w0);
        const size_t  H = alea.leq(w0);
        const RGB     C = NamedColor::FetchRGB(alea.leq(1000));
        const float   F = Convert::RGB2F(C);
        const uint8_t U = Convert::RGB2U(C);

        const unit_t X0 = x0 + alea.leq(w0);
        const unit_t X1 = x0 + alea.leq(w0);
        const unit_t Y0 = y0 + alea.leq(h0);
        const unit_t Y1 = y0 + alea.leq(h0);

        if( alea.get<float>() > 0.5f )
        {
            Draw::HLine(img3,X,Y,W,C);
            Draw::HLine(imgf,X,Y,W,F);
            Draw::HLine(img1,X,Y,W,U);

            Draw::VLine(img3,X,Y,H,C);
            Draw::VLine(imgf,X,Y,H,F);
            Draw::VLine(img1,X,Y,H,U);

            Draw::Line(img3,X0,Y0,X1,Y1,C);
            Draw::Line(imgf,X0,Y0,X1,Y1,F);
            Draw::Line(img1,X0,Y0,X1,Y1,U);

        }
        else
        {
            const uint8_t alpha = alea.leq(255);
            Draw::HLine(img3,X,Y,W,C,alpha);
            Draw::HLine(imgf,X,Y,W,F,alpha);
            Draw::HLine(img1,X,Y,W,U,alpha);
            Draw::VLine(img3,X,Y,H,C,alpha);
            Draw::VLine(imgf,X,Y,H,F,alpha);
            Draw::VLine(img1,X,Y,H,U,alpha);
            
            Draw::Line(img3,X0,Y0,X1,Y1,C,alpha);
            Draw::Line(imgf,X0,Y0,X1,Y1,F,alpha);
            Draw::Line(img1,X0,Y0,X1,Y1,U,alpha);
        }
    }

    for(size_t k=0;k<10;++k)
    {
        const unit_t X0 = x0 + alea.leq(w0);
        const unit_t X1 = x0 + alea.leq(w0);
        const unit_t Y0 = y0 + alea.leq(h0);
        const unit_t Y1 = y0 + alea.leq(h0);
        const RGB     C = NamedColor::FetchRGB(alea.leq(1000));
        const float   F = Convert::RGB2F(C);
        const uint8_t U = Convert::RGB2U(C);
        if( alea.get<float>() > 0.9f )
        {
            Draw::Fill(img3, X0, Y0, X1, Y1, C);
            Draw::Fill(imgf, X0, Y0, X1, Y1, F);
            Draw::Fill(img1, X0, Y0, X1, Y1, U);

        }
        else
        {
            const uint8_t alpha = alea.leq(255);
            Draw::Fill(img3, X0, Y0, X1, Y1, C, alpha);
            Draw::Fill(imgf, X0, Y0, X1, Y1, F, alpha);
            Draw::Fill(img1, X0, Y0, X1, Y1, U, alpha);

        }
    }
    
    for(size_t k=0;k<100;++k)
    {
        const unit_t  X = x0 + alea.leq(w0);
        const unit_t  Y = y0 + alea.leq(h0);
        const size_t  R = alea.leq(w0);
        const RGB     C = NamedColor::FetchRGB(alea.leq(1000));
        const float   F = Convert::RGB2F(C);
        const uint8_t U = Convert::RGB2U(C);
        if( alea.get<float>() > 0.8f )
        {
            Draw::Circle(img3, X, Y, R, C);
            Draw::Circle(imgf, X, Y, R, F);
            Draw::Circle(img1, X, Y, R, U);
        }
        else
        {
            const uint8_t alpha = alea.leq(255);
            Draw::Circle(img3, X, Y, R, C, alpha);
            Draw::Circle(imgf, X, Y, R, F, alpha);
            Draw::Circle(img1, X, Y, R, U, alpha);
        }
        
    }
    
    for(size_t k=0;k<50;++k)
    {
        const unit_t  X = x0 + alea.leq(w0);
        const unit_t  Y = y0 + alea.leq(h0);
        const size_t  R = alea.leq(100);
        const RGB     C = NamedColor::FetchRGB(alea.leq(1000));
        const float   F = Convert::RGB2F(C);
        const uint8_t U = Convert::RGB2U(C);
        if( alea.get<float>() > 0.9f )
        {
            Draw::Disk(img3, X, Y, R, C);
            Draw::Disk(imgf, X, Y, R, F);
            Draw::Disk(img1, X, Y, R, U);
        }
        else
        {
            const uint8_t alpha = alea.leq(255);
            Draw::Disk(img3, X, Y, R, C, alpha);
            Draw::Disk(imgf, X, Y, R, F, alpha);
            Draw::Disk(img1, X, Y, R, U, alpha);
        }
    }

    IMG.save(img3, "img3.png", NULL);
    IMG.save(imgf, "imgf.png", NULL);
    IMG.save(img1, "img1.png", NULL);

}
YOCTO_UNIT_TEST_DONE()

