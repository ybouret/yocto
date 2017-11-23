#include "yocto/randomized/generator.hpp"
#include "yocto/randomized/uniform0.hpp"
#include "yocto/randomized/uniform1.hpp"
#include "yocto/randomized/uniform2.hpp"
#include "yocto/randomized/uniform3.hpp"
#include "yocto/randomized/uniform64.hpp"

#include "yocto/randomized/isaac.hpp"


#include "yocto/sys/timings.hpp"

#include "yocto/utest/run.hpp"
#include <cstdlib>
#include <math.h>
#include <cstdio>

using namespace yocto;
using namespace Randomized;

#define N 10000

static inline
void test_rg( Bits &bits, const char *name )
{
    std::cerr << "<" << name << ">" << std::endl;

    //bits.reseed( Bits::Simple() );

    {
        double sum    = 0;
        double sum_sq = 0;
        for(size_t i=0;i<N;++i)
        {
            const double x = bits.to<double>();
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
        float sum    = 0;
        float sum_sq = 0;
        for(size_t i=0;i<N;++i)
        {
            const double x = bits.symm<float>();
            sum    += x;
            sum_sq += x*x;
        }
        const float ave = sum/N;
        const float var = (sum_sq - sum*sum/N)/(N-1);
        const float sig = sqrtf(var);
        std::cerr << "ave0=" << ave << std::endl;
        std::cerr << "sig0=" << sig << std::endl;
    }

    timings tmx;
    volatile uint32_t r32=0;
    YOCTO_TIMINGS(tmx,0.5,r32=bits.next32());
    std::cerr << "\t\tspeed = " << tmx.speed*1e-6 << " Mops" << std::endl;

}

#define __IMPL(TYPE) do { TYPE rg; test_rg(rg,#TYPE " 1/2"); rg.reseed(Bits::Simple()); test_rg(rg,#TYPE " 2/2"); } while(false)

YOCTO_UNIT_TEST_IMPL(randomized)
{
    __IMPL(cstdbits);
    __IMPL(Uniform0);
    __IMPL(Uniform1);
    __IMPL(Uniform2);
    __IMPL(Uniform3);
    __IMPL(Uniform64NR);
    __IMPL(Uniform64BJ);
    __IMPL(ISAAC<4>);
    __IMPL(ISAAC<8>);


    Generator<double,cstdbits> ran;

    for(size_t i=0;i<10;++i)
    {
        std::cerr << "\t" << ran() << std::endl;
    }

}
YOCTO_UNIT_TEST_DONE()

