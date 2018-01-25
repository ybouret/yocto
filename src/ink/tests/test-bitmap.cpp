#include "yocto/ink/pixmap.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;

#define __ALEA_SIZE 10+alea.leq(900),10+alea.leq(900)

template <typename T>
static inline
void fill_pxm( Ink::Pixmap<T> &pxm ) throw()
{
    for(unit_t j=0;j<pxm.h;++j)
    {
        for(unit_t i=0;i<pxm.w;++i)
        {
            T tmp = pxm[j][i];
            tmp   = T( alea.leq(1000) );
            pxm[j][i] = tmp;
        }
    }

    Ink::Pixmap<T> cpy( pxm );
    for(unit_t j=0;j<pxm.h;++j)
    {
        for(unit_t i=0;i<pxm.w;++i)
        {
            cpy[j][i] = 0;
        }
    }
}

#define __SHOW(TYPE) std::cerr << "sizeof(" << #TYPE << ")\t= " << sizeof(TYPE) << std::endl

struct color3 { uint8_t r,g,b; };

YOCTO_UNIT_TEST_IMPL(bitmap)
{
    __SHOW(Ink::Bitmap);
    __SHOW(Ink::Pixmap<char>   );
    __SHOW(Ink::Pixmap<short>  );
    __SHOW(Ink::Pixmap<float>  );
    __SHOW(Ink::Pixmap<double> );

    std::cerr << "----> testing bitmaps" << std::endl;
    Ink::Bitmap bmp1(1,__ALEA_SIZE);
    Ink::Bitmap bmp2(2,__ALEA_SIZE);
    Ink::Bitmap bmp3(3,__ALEA_SIZE);
    Ink::Bitmap bmp4(4,__ALEA_SIZE);
    Ink::Bitmap bmp8(8,__ALEA_SIZE);

    std::cerr << "----> testing pixmaps" << std::endl;
    Ink::Pixmap<char>   pxm_char(__ALEA_SIZE);
    Ink::Pixmap<short>  pxm_short(__ALEA_SIZE);
    Ink::Pixmap<float>  pxmf(__ALEA_SIZE);
    Ink::Pixmap<double> pxmd(__ALEA_SIZE);

    pxm_char.ldz();
    pxm_short.ldz();
    pxmf.ldz();
    pxmd.ldz();

    fill_pxm(pxm_char);
    fill_pxm(pxm_short);
    fill_pxm(pxmf);
    fill_pxm(pxmd);


    std::cerr << "----> testing shared" << std::endl;
    Ink::Bitmap sh( new Ink::Bitmap(1,__ALEA_SIZE) );
    Ink::Bitmap sc( sh );
    sh.ldz();

    std::cerr << "----> testing save" << std::endl;
    Ink::Pixmap<color3> pxm(__ALEA_SIZE);
    for(unit_t j=0;j<pxm.h;++j)
    {
        uint8_t r( alea.leq(255) ), g( 0*alea.leq(255) ), b( 0*alea.leq(255) );
        for(unit_t i=0;i<pxm.w;++i)
        {
            color3 &C = pxm[j][i];
            C.r = r;
            C.b = b;
            C.g = g;
        }
    }
    pxm.save("pxm.bmp");
    pxm.save("pxm.ppm");
    pxm.save("pxm.eps");
    
}
YOCTO_UNIT_TEST_DONE()

