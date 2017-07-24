#include "yocto/gfx/image.hpp"
#include "yocto/gfx/image/jpeg.hpp"
#include "yocto/gfx/image/png.hpp"
#include "yocto/gfx/image/tiff.hpp"

#include "yocto/gfx/ops/geometry.hpp"
#include "yocto/gfx/draw/stencil.hpp"


#include "yocto/utest/run.hpp"
#include "yocto/math/trigconv.hpp"
#include "yocto/gfx/color/named-colors.hpp"
#include "yocto/code/rand.hpp"

using namespace yocto;
using namespace gfx;

YOCTO_UNIT_TEST_IMPL(draw)
{
    YOCTO_GFX_DECL_FORMAT(jpeg);
    YOCTO_GFX_DECL_FORMAT(png);
    YOCTO_GFX_DECL_FORMAT(tiff);

    imageIO          &IMG = image::instance();


    pixmap<RGB> img(800,600);


    const unit_t w  = img.w;
    const size_t dw = w+w;
    const unit_t h  = img.h;
    const size_t dh = h+h;
    const size_t dm = max_of(dh,dw);

    img.ldz();
    for(size_t k=0;k<100;++k)
    {
        unit_t x0 = -w + unit_t( alea_leq(dw) );
        unit_t x1 = -w + unit_t( alea_leq(dw) );
        unit_t y0 = -h + unit_t( alea_leq(dh) );
        unit_t y1 = -h + unit_t( alea_leq(dh) );
        draw_line(img,x0,y0,x1,y1, named_color::fetch(alea_leq(named_color::count)),128+alea_leq(127));
    }

    for(size_t k=0;k<100;++k)
    {
        unit_t x0 = -w + unit_t( alea_leq(dw) );
        unit_t y0 = -h + unit_t( alea_leq(dh) );
        unit_t r  = unit_t(alea_leq(dm));
        draw_circle(img, x0, y0,r,  named_color::fetch(alea_leq(named_color::count)),128+alea_leq(127));
    }

    for(size_t k=0;k<10;++k)
    {
        unit_t x0 = -w + unit_t( alea_leq(dw) );
        unit_t y0 = -h + unit_t( alea_leq(dh) );
        unit_t r  = unit_t(alea_leq(dm));
        r=5*k;
        draw_disk(img, x0, y0,r,  named_color::fetch(alea_leq(named_color::count)),128+alea_leq(127));
    }

    for(size_t k=0;k<4;++k)
    {
        unit_t x0 =  unit_t( alea_leq(w) );
        unit_t y0 =  unit_t( alea_leq(h) );
        vector<vertex> sten(1000,as_capacity);
        collect_disk(sten, x0, y0, 10 + alea_leq(100) );
        clean_stencil(sten);
        for(size_t num=4+alea_leq(4);num>0;--num)
        {
            const size_t m = 4 + alea_leq(8);
            draw_stencil(sten, img, vertex(num*m,num*m),  named_color::fetch(alea_leq(named_color::count)),128+alea_leq(127) );
        }
    }


    for(size_t k=0;k<4;++k)
    {
        unit_t x0 =  unit_t( alea_leq(w) );
        unit_t y0 =  unit_t( alea_leq(h) );
        vector<vertex> sten(1000,as_capacity);
        collect_disk(sten, x0, y0, 4 + alea_leq(4) );
        clean_stencil(sten);
        const vertex shift( -100 + unit_t( alea_leq(200) ), -100 + unit_t(alea_leq(200)));
        extrude_stencil(sten,shift);
        std::cerr << "sten.capacity=" << sten.capacity() << std::endl;
        draw_stencil(sten,img,shift,named_color::fetch(alea_leq(named_color::count)),128+alea_leq(127));
    }


    IMG.save("draw.png",img,0);

}
YOCTO_UNIT_TEST_DONE()
