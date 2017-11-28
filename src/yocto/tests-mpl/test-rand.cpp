#include "yocto/utest/run.hpp"
#include "yocto/mpl/natural.hpp"

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
    }
}
YOCTO_UNIT_TEST_DONE()

