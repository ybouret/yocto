
#include "yocto/ink/ops/fft.hpp"
#include "yocto/ink/ops/mapper.hpp"
#include "yocto/ink/color/conv.hpp"
#include "yocto/ink/image.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ink/color/ramp/cold_to_very_hot.hpp"


using namespace yocto;
using namespace Ink;

YOCTO_UNIT_TEST_IMPL(fft)
{
    YOCTO_IMG();

    Engine::SharedServer seqSrv( new threading::seq_server() );
    Engine::SharedServer parSrv( new threading::par_server() );


    Mapper mapper;

    if(argc>1)
    {
        const PixmapRGB img( IMG.loadRGB(argv[1],NULL) );
        IMG.save(img,"img.png",NULL);
        Engine par(img,parSrv);

        const Area ap2 = FFT::ComputeAreaFor(img);
        PixmapZ    zimg(ap2.w,ap2.h);
        FFT        fft;

        fft.load(zimg,img, Convert::RGB2F, par);

        float cMin=0,cMax=0;
        mapper.getMinMax(cMin, cMax, zimg, Convert::CPLX2MOD, par);
        {
            ramp_cold_to_very_hot c2c(cMin,cMax);
            IMG.save("zimg.png", zimg, c2c, NULL);
        }

        std::cerr << "\t\tFFT forward" << std::endl;
        fft.forward(zimg);
        mapper.getMinMax(cMin, cMax, zimg, Convert::CPLX2MOD, par);
        std::cerr << "cmin=" << cMin << ", cmax=" << cMax << std::endl;
        {
            ramp_cold_to_very_hot c2c(cMin,cMax);
            IMG.save("zfwd.png", zimg, c2c, NULL);
        }

        std::cerr << "\t\tFFT reverse" << std::endl;
        fft.reverse(zimg);
        mapper.getMinMax(cMin, cMax, zimg, Convert::CPLX2MOD, par);
        std::cerr << "cmin=" << cMin << ", cmax=" << cMax << std::endl;
        {
            ramp_cold_to_very_hot c2c(cMin,cMax);
            IMG.save("zrev.png", zimg, c2c, NULL);
        }

        PixmapF pxm(img.w,img.h);
        fft.save(pxm,zimg,Convert::CPLX2RE,par);
        IMG.save(pxm,"imre.png", NULL);
    }

}
YOCTO_UNIT_TEST_DONE()

