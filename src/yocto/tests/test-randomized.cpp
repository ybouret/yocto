#include "yocto/randomized/generator.hpp"
#include "yocto/randomized/uniform0.hpp"
#include "yocto/randomized/uniform1.hpp"
#include "yocto/randomized/uniform2.hpp"
#include "yocto/randomized/uniform3.hpp"
#include "yocto/randomized/uniform64.hpp"
#include "yocto/randomized/uniform-mt.hpp"
#include "yocto/randomized/urand32.hpp"
#include "yocto/code/alea.hpp"

#include "yocto/randomized/isaac.hpp"


#include "yocto/sys/timings.hpp"

#include "yocto/utest/run.hpp"
#include <cstdlib>
#include <math.h>
#include <cstdio>

using namespace yocto;
using namespace Randomized;

static const size_t NUM=10000;

static inline
void test_rg( Bits &bits, const char *name )
{
    std::cerr << "<" << name << ">: 0.." << bits.span << " => bits=" << bits.bits << std::endl;
    
    //bits.reseed( Bits::Simple() );

    {
        double sum    = 0;
        double sum_sq = 0;
        for(size_t i=0;i<NUM;++i)
        {
            const double x = bits.to<double>();
            sum    += x;
            sum_sq += x*x;
        }
        const double ave = sum/NUM;
        const double var = (sum_sq - sum*sum/NUM)/(NUM-1);
        const double sig = sqrt(var);
        std::cerr << "ave1=" << ave << ",\tsig1=" << sig << std::endl;
    }

    {
        float sum    = 0;
        float sum_sq = 0;
        for(size_t i=0;i<NUM;++i)
        {
            const double x = bits.symm<float>();
            sum    += x;
            sum_sq += x*x;
        }
        const float ave = sum/NUM;
        const float var = (sum_sq - sum*sum/NUM)/(NUM-1);
        const float sig = sqrtf(var);
        std::cerr << "ave0=" << ave << ",\tsig0=" << sig << std::endl;
    }

    for(size_t i=0;i<10;++i)
    {
        const uint64_t u64 = bits.full<uint64_t>();
        const uint64_t u16 = bits.full<uint16_t>();
        std::cerr << "\tu16=" << u16 << ", u64=" << u64 << std::endl;
    }
    
    timings tmx;
    volatile uint32_t r32=0;
    YOCTO_TIMINGS(tmx,1,r32=bits.next32());
    std::cerr << "\tspeed = " << tmx.speed*1e-6 << " Mops" << std::endl;
    std::cerr << std::endl;
}

#define __IMPL(TYPE) do { TYPE rg; test_rg(rg,#TYPE); } while(false)

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
    __IMPL(UniformMT);
    __IMPL(Kiss32);
    __rand32 rr;
    rr.test();

    Generator<double,cstdbits> ran;

    for(size_t i=0;i<10;++i)
    {
        std::cerr << "\t" << ran() << std::endl;
    }

}
YOCTO_UNIT_TEST_DONE()

#define ALEA_GET(TYPE) std::cerr << #TYPE << "\t " << alea.get<TYPE>() << std::endl
#include "yocto/string.hpp"

#include "yocto/sort/quick.hpp"
#include "yocto/sequence/vector.hpp"

YOCTO_UNIT_TEST_IMPL(alea)
{
    alea.initialize();
    for(size_t i=0;i<10;++i)
    {
        ALEA_GET(float);
        ALEA_GET(double);
        ALEA_GET(int);
        ALEA_GET(string);
    }
    vector<string> strings;
    for(size_t i=5+alea.leq(20);i>0;--i)
    {
        const string tmp = alea.get<string>();
        strings.push_back(tmp);
    }
    quicksort(strings);
    vector<size_t> indices(strings.size());

    for(size_t i=1;i<=strings.size();++i)
    {
        indices[i] = i;
    }
    std::cerr << "strings=" << strings << std::endl;
    std::cerr << "indices=" << indices << std::endl;
    std::cerr << std::endl;
    alea.shuffle(strings(),indices(),strings.size());
    std::cerr << "strings=" << strings << std::endl;
    std::cerr << "indices=" << indices << std::endl;
    std::cerr << std::endl;

    co_qsort(indices,strings);
    std::cerr << "strings=" << strings << std::endl;
    std::cerr << "indices=" << indices << std::endl;
    std::cerr << std::endl;

}
YOCTO_UNIT_TEST_DONE()

