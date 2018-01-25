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
}

YOCTO_UNIT_TEST_IMPL(bitmap)
{
    Ink::Bitmap bmp1(1,__ALEA_SIZE);
    Ink::Bitmap bmp2(2,__ALEA_SIZE);
    Ink::Bitmap bmp3(3,__ALEA_SIZE);
    Ink::Bitmap bmp4(4,__ALEA_SIZE);
    Ink::Bitmap bmp8(8,__ALEA_SIZE);

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
}
YOCTO_UNIT_TEST_DONE()

