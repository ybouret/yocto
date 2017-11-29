#include "yocto/utest/run.hpp"
#include "yocto/mpl/rational.hpp"

using namespace yocto;

YOCTO_UNIT_TEST_IMPL(rand)
{
    for(size_t bits=0;bits<=128;bits+=1+alea.lt(8))
    {
        std::cerr << std::dec << "bits=" << bits << std::endl;
        for(size_t j=0;j<8;++j)
        {
            mpn m = mpn::rand(bits);
            std::cerr << std::hex << "\tm=" << m << ", bits=" << std::dec << m.bits() << std::endl;


        }

        mpn ave = 0;
        mpn n   = 0;
        for(size_t i=10000;i>0;--i)
        {
            ave += mpn::rand(bits);
            ++n;
        }
        ave /= n;
        std::cerr << std::hex << "ave = " << ave << std::endl;
#if 1
        if(bits>0)
        {
            const mpn expected = ((mpn::exp2(bits-1)-1)>>1) | (mpn::exp2(bits-1));
            std::cerr << "for = " << expected << std::endl;
            mpq ratio(ave,expected);
            std::cerr << "ratio=" << ratio.to_double() << std::endl;
        }
#endif
    }
}
YOCTO_UNIT_TEST_DONE()

