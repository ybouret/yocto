#include "yocto/mpl/natural.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/code/utils.hpp"
#include "yocto/code/alea.hpp"
#include "yocto/sys/timings.hpp"
#include "yocto/sort/quick.hpp"

using namespace yocto;
using namespace mpl;

#define _SHOW(X) std::cerr << #X " = " << X << std::endl

YOCTO_UNIT_TEST_IMPL(mul)
{
#if 0
    mpn x = mpn::rand(100);
    mpn y = mpn::rand(40);
    mpn z = x*y;
    
    return 0;
#endif
    
    const size_t bits_min    = 256;
    const size_t bits_max    = 1024;
    const size_t bits_del    = bits_max - bits_min + 1;
    const size_t num_per_bit = 4;
    const size_t count       = bits_del * num_per_bit;
    vector<mpn>  v(count,as_capacity);

    std::cerr << "-- Generating " << count << " numbers..." << std::endl;



    v.free();
    for(size_t i=bits_min;i<=bits_max;++i)
    {
        for(size_t j=num_per_bit;j>0;--j)
        {
            const mpn x = mpn::rand(i);
            v.push_back(x);
        }
    }
    alea.shuffle(v(),v.size());
    assert(v.size()==count);
    std::cerr << "-- Checking products" << std::endl;
    for(size_t i=1,j=count;i<=count;++i,--j)
    {
        const mpn p1 = mpn::mul(v[i].ro(),v[i].length(),v[j].ro(),v[j].length());
        const mpn p2 = mpn::mul_v2(v[i].ro(),v[i].length(),v[j].ro(),v[j].length());
        if(p1!=p2)
        {
            throw exception("invalid multiplication");
        }
    }

    mpn s = 0;

    double time_length = 5;
    std::cerr << "-- Starting MUL for " << time_length << " seconds" << std::endl;

    timings tmx;
    YOCTO_TIMINGS(tmx, time_length,
                  s.ldz();
                  for(size_t i=1,j=count;i<=count;++i,--j)
                  {
                      //s = v[i]*v[j];
                      s = mpn::mul(v[i].ro(),v[i].length(),v[j].ro(),v[j].length());
                  }
                  );
    tmx.speed *= count;
    tmx.speed *= 1e-3;
    std::cerr << "speed=" << tmx.speed << " kops" << std::endl;

    std::cerr << "-- Starting MUL_V2 for " << time_length << " seconds" << std::endl;

    YOCTO_TIMINGS(tmx, time_length,
                  s.ldz();
                  for(size_t i=1,j=count;i<=count;++i,--j)
                  {
                      //s = v[i]*v[j];
                      s = mpn::mul_v2(v[i].ro(),v[i].length(),v[j].ro(),v[j].length());
                  }
                  );
    tmx.speed *= count;
    tmx.speed *= 1e-3;
    std::cerr << "speed=" << tmx.speed << " kops" << std::endl;

}
YOCTO_UNIT_TEST_DONE()
