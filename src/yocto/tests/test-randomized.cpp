#include "yocto/randomized/generator.hpp"
#include "yocto/randomized/uniform0.hpp"
#include "yocto/randomized/uniform1.hpp"
#include "yocto/randomized/uniform2.hpp"
#include "yocto/randomized/uniform3.hpp"
#include "yocto/randomized/uniform64.hpp"
#include "yocto/randomized/uniform-mt.hpp"
#include "yocto/randomized/urand32.hpp"
#include "yocto/code/alea.hpp"
#include "yocto/randomized/gaussian.hpp"

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

#include "yocto/associative/map.hpp"
static inline
void rand_quality( Bits &bits, const char *name)
{
    static const double count32 = 4294967296.0;
    static const size_t min_cap = 2097152;
    static const size_t max_try = 4194304;

    std::cerr << "Quality of " << name << std::endl;
    map<uint32_t,size_t> counts(min_cap,as_capacity);
    const size_t nmax = counts.capacity();
    std::cerr << "nmax=" << nmax << std::endl;

    std::cerr << "...generating" << std::endl;
    for(size_t i=0;i<max_try;++i)
    {
        const uint32_t r = bits.next32();
        size_t        *p = counts.search(r);
        if(p)
        {
            ++(*p);
        }
        else
        {
            if(counts.size()>=nmax)
            {
                std::cerr << "full counts @" << i+1 << "/" << nmax << std::endl;
                break;
            }
            else
            {
                if(!counts.insert(r,1))
                {
                    throw exception("unexpected insertion failure");
                }
            }
        }
    }
    std::cerr << "...done" << std::endl;

    double E = (double(nmax)/count32) * log(1.0/count32);
    std::cerr << "...entropy (default=" << -E << ")" << std::endl;
    for(map<uint32_t,size_t>::iterator it=counts.begin();it!=counts.end();++it)
    {
        const size_t ni = *it; assert(ni>0);
        const double pi = double(ni)/count32;
        E -= log(pi)*pi;
    }
    std::cerr << "Exs=" << E*1.0e6 << " ppm" << std::endl;

    std::cerr << std::endl;
}

#define __QC(TYPE) do { TYPE rg; rg.reseed(alea); rand_quality(rg,#TYPE); } while(false)
YOCTO_UNIT_TEST_IMPL(randQC)
{
    __QC(cstdbits);
    __QC(Kiss32);
    __QC(UniformMT);
    __QC(ISAAC<4>);
    __QC(ISAAC<8>);

}
YOCTO_UNIT_TEST_DONE()

#define ALEA_GET(TYPE) std::cerr << #TYPE << "\t " << alea.get<TYPE>() << std::endl
#include "yocto/string.hpp"

#include "yocto/sort/quick.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/math/point3d.hpp"

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

    {
        ios::wcstream fp("circle.dat");
        point2d<float> g;
        size_t         n = 0;
        for(size_t i=100+alea.leq(1000);i>0;--i)
        {
            point2d<float> p = alea.onCircle< point2d<float> >();
            fp("%g %g\n",p.x,p.y);
            g += p;
            ++n;
        }
        g /= float(n);
        fp("%g %g\n",g.x,g.y);
    }

    {
        ios::wcstream fp("disk.dat");
        for(size_t i=100+alea.leq(1000);i>0;--i)
        {
            point2d<double> p = alea.inDisk< point2d<double> >();
            fp("%g %g\n",p.x,p.y);
        }
    }

    {
        ios::wcstream fp("sphere.dat");
        point3d<float> g;
        size_t         n = 0;
        for(size_t i=100+alea.leq(1000);i>0;--i)
        {
            point3d<float> p = alea.onSphere< point3d<float> >();
            fp("%g %g %g\n",p.x,p.y,p.z);
            g += p;
            ++n;
        }
        g /= float(n);
    }

    {
        ios::wcstream fp("ball.dat");
        for(size_t i=100+alea.leq(1000);i>0;--i)
        {
            point3d<double> p = alea.inBall< point3d<double> >();
            fp("%g %g %g\n",p.x,p.y,p.z);
        }
    }



    }
    YOCTO_UNIT_TEST_DONE()

    template <typename GEN>
    static inline void test_Gen(GEN &G)
    {
        typedef typename GEN::type type;
        type sum = 0;
        type sum_sq = 0;
        size_t n = 0;
        for(size_t i=100000;i>0;--i)
        {
            const type x = G();
            sum    += x;
            sum_sq += x*x;
            ++n;
        }
        const double ave = sum/n;
        const double var = (sum_sq - sum*sum/n)/(n-1);
        const double sig = sqrt(var);
        std::cerr << "ave=" << ave << std::endl;
        std::cerr << "sig=" << sig << std::endl;

    }
    
#include "yocto/randomized/bivariate-gaussian.hpp"

    YOCTO_UNIT_TEST_IMPL(gaussian)
    {
        GaussianGenerator<double,UniformMT> Gd;
        GaussianGenerator<float,UniformMT>  Gf;
        Gd.reseed(alea);
        Gf.reseed(alea);
        test_Gen(Gd);
        test_Gen(Gf);

        BivariateGaussian<double,Uniform2> BG(1,2,0.1);
        //BG.reseed(alea);

        double sum1=0,sum2=0,sq1=0,sq2=0;
        size_t n = 0;
        for(size_t i=1000;i>0;--i)
        {
            double u1,u2;
            BG(u1,u2);
            sum1 += u1;
            sq1  += u1*u1;
            sum2 += u2;
            sq2  += u2*u2;
            ++n;
        }
        
        const double ave1 = sum1/n;
        const double ave2 = sum2/n;
        const double var1 = (sq1 - sum1*sum1/n)/(n-1);
        const double var2 = (sq2 - sum2*sum2/n)/(n-1);
        const double sig1 = sqrt(var1);
        const double sig2 = sqrt(var2);
        std::cerr << "ave1=" << ave1 << ", sig1=" << sig1 << std::endl;
        std::cerr << "ave2=" << ave2 << ", sig2=" << sig2 << std::endl;
        
    }
    YOCTO_UNIT_TEST_DONE()



