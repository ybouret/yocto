#include "yocto/randomized/bits.hpp"
#include "yocto/utest/run.hpp"
#include <cstdlib>
#include <math.h>
#include <cstdio>

using namespace yocto;
using namespace Randomized;

#define N 1000

static inline
void test_rg( Bits &bits, const char *name )
{
    std::cerr << "<" << name << ">" << std::endl;

    {
        double sum    = 0;
        double sum_sq = 0;
        for(size_t i=0;i<N;++i)
        {
            const double x = bits.nextDouble();
            sum    += x;
            sum_sq += x*x;
        }
        const double ave = sum/N;
        const double var = (sum_sq - sum*sum/N)/(N-1);
        const double sig = sqrt(var);
        std::cerr << "ave1=" << ave << std::endl;
        std::cerr << "sig1=" << sig << std::endl;
    }

    {
        double sum    = 0;
        double sum_sq = 0;
        for(size_t i=0;i<N;++i)
        {
            const double x = bits.nextDoubleSymm();
            sum    += x;
            sum_sq += x*x;
        }
        const double ave = sum/N;
        const double var = (sum_sq - sum*sum/N)/(N-1);
        const double sig = sqrt(var);
        std::cerr << "ave1=" << ave << std::endl;
        std::cerr << "sig1=" << sig << std::endl;
    }
}


YOCTO_UNIT_TEST_IMPL(randomized)
{
    
    cstdbits cran;
    test_rg(cran,"cstdbits");
    
}
YOCTO_UNIT_TEST_DONE()

