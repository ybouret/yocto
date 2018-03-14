
#include "yocto/ink/ops/edges-detector.hpp"
#include "yocto/ink/color/conv.hpp"
#include "yocto/ink/color/ramp/cold_to_very_hot.hpp"
#include "yocto/ink/color/ramp/cold_to_cold.hpp"
#include "yocto/ink/color/ramp/grey.hpp"
#include "yocto/ink/ops/mapper.hpp"
#include "yocto/ink/ops/filter.hpp"
#include "yocto/ink/ops/sobel.hpp"
#include "yocto/ink/ops/scharr.hpp"

#include "yocto/ink/image.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/string/conv.hpp"

#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace Ink;

static inline void compute_edges(Edges               &edges,
                                 const Pixmap<float> &img,
                                 const Stencil       &dx,
                                 const Stencil       &dy,
                                 Engine              &engine,
                                 Blur                *blur)
{
    YOCTO_IMG_IO();
    if(blur)
    {
        std::cerr << "BLUR: " << std::endl;
    }
    else
    {
        std::cerr << "NO BLUR" << std::endl;
    }
    edges.loadIntensity(img, blur, engine);
    const string sfx = dx.name + '-' + dy.name;
    edges.computeGradient(dx, dy, engine);
    {
        ramp_cold_to_very_hot gRamp(0,edges.gradient.v_max);
        const string fn = "grad-" + sfx + ".png";
        IMG.save(fn, edges.gradient.value, gRamp, NULL);
    }
    edges.keepLocalMaxima(engine);
    {
        const string fn = "maxima-" + sfx + ".png";
        IMG.save(edges.maxima,fn,NULL);
    }
    std::cerr << "strong=" << edges.strong << std::endl;
    std::cerr << "weak  =" << edges.weak   << std::endl;


    edges.statisticalLevels(engine);
    {
        const string fn = "grade-" + sfx + ".png";
        IMG.save(edges,fn,NULL);
    }

    Particles particles;
    edges.build(particles);
    {
        const string fn = "edges-" + sfx + ".png";
        IMG.save(edges,fn,NULL);
    }

    {
        indx2rgba<size_t> tagColors(10);
        const string fn = "tags-" + sfx + ".png";
        IMG.save(fn,edges.tags,tagColors,NULL);
    }

    std::cerr << "#edges=" << particles.size << std::endl;

}

YOCTO_UNIT_TEST_IMPL(edges)
{
    YOCTO_IMG();

    if(false)
    {
        ios::wcstream fp("probe.dat");
        const float radius = 1.0;
        for(float theta=0;theta<=6.3f;theta+=0.05f)
        {
            const float x  = radius * cos(theta);
            const float y  = radius * sin(theta);
            const float dx = floorf( x + 0.5f );
            const float dy = floorf( y + 0.5f );
            fp("%g %g\n%g %g\n\n", x, y, dx, dy);
        }

    }

    Engine::SharedServer   seqSrv( new threading::seq_server() );
    Engine::SharedServer   parSrv( new threading::par_server() );
    Mapper                 mapper;
    if(argc>1)
    {
        const PixmapRGB pxm3( IMG.loadRGB(argv[1],NULL) );
        Engine          par(pxm3,parSrv);
        Pixmap<float>   pxmf( pxm3.w, pxm3.h );
        mapper(pxmf,pxm3,Convert::RGB2F,par);
        IMG.save(pxm3, "img3.png", NULL);
        IMG.save(pxmf, "imgf.png", NULL);

        auto_ptr<Blur> blur;
        if(argc>2)
        {
            blur.reset( new BlurOn<5>( strconv::to<float>(argv[2]) ) );
        }
        Edges edges(pxm3.w,pxm3.h);

        {
            Sobel3X dx;
            Sobel3Y dy;
            compute_edges(edges,pxmf,dx,dy,par,blur.__get());
        }

        {
            Sobel5X dx;
            Sobel5Y dy;
            compute_edges(edges,pxmf,dx,dy,par,blur.__get());
        }

        {
            Scharr3X dx;
            Scharr3Y dy;
            compute_edges(edges,pxmf,dx,dy,par,blur.__get());
        }

        {
            Scharr5X dx;
            Scharr5Y dy;
            compute_edges(edges,pxmf,dx,dy,par,blur.__get());
        }

    }

#if 0
    BlurOn<5>         g5x5(1.4f);
    ramp_cold_to_cold armp(-math::numeric<float>::pi,math::numeric<float>::pi);
    std::cerr << "atan2(0,1)=" << math::Atan2(0.0f,1.0f) << std::endl;
    if(argc>1)
    {
        const PixmapRGB pxm3( IMG.loadRGB(argv[1],NULL) );
        Pixmap<float>   pxmf( pxm3, Convert::RGB2F );
        IMG.save(pxm3, "img3.png", NULL);
        IMG.save(pxmf, "imgf.png", NULL);
        Engine par(pxm3,parSrv);
        Edges  edges(pxm3.w,pxm3.h);
        g5x5.apply(edges.intensity,pxmf,par);
        IMG.save(edges.intensity, "edges_intensity.png", NULL);
        edges.computeGradient(par);

        //ramp_cold_to_very_hot grmp(0.0f, edges.Gmax );
        ramp_grey  grmp(0.0f, edges.Gmax );

        IMG.save("edges_gradient.png",  edges.gradient,  grmp, NULL);
        IMG.save("edges_direction.png", edges.direction, armp, NULL);

        edges.keepLocalMaxima(par);
        std::cerr << "strong=" << edges.strong << std::endl;
        std::cerr << "weak="   << edges.weak   << std::endl;
        
        IMG.save(edges.maxima,"edges_maxima.png",NULL);

        PixmapRGB g3(pxm3.w,pxm3.h);
        mapper(g3,edges.maxima,pxm3,Convert::dimRGB,par);
        IMG.save(g3,"edges_maxima3.png",NULL);

        edges.grade(par);
        indx2rgba<uint8_t> edgeColors(0);
        IMG.save("edges_grade.png",edges,edgeColors,NULL);


        Particles particles;
        edges.connect(particles,par);
        indx2rgba<size_t> blobColors(alea.leq(NamedColor::DB_SIZE));
        IMG.save("edges_tags.png", edges.tags, blobColors, NULL);

        IMG.save("edges_map.png",edges,edgeColors,NULL);
        
        mapper(g3,edges,pxm3,Convert::dimRGB,par);
        IMG.save(g3,"edges_rgb.png",NULL);

        AutoFilter<uint8_t> F(edges);
        F.Close(edges,par);
        //F.Average(edges,par);
        // IMG.save(edges,"edges_map_filter.png",NULL);
        IMG.save("edges_map_filter.png",edges,edgeColors,NULL);
    }
#endif


}
YOCTO_UNIT_TEST_DONE()


