#include "yocto/utest/run.hpp"
#include "yocto/code/alea.hpp"

using namespace yocto;



#include "yocto/code/primality.hpp"
#include "yocto/string/conv.hpp"
#include "yocto/sys/timings.hpp"

static inline
double check_perf(const size_t n, bool (*proc)(size_t n) )
{
    assert(proc);
    timings tmx;
    YOCTO_TIMINGS(tmx,3.0,
                  for(size_t i=0;i<=n;++i)
                  {
                      (void) proc(i);
                  }
                  );
    return tmx.speed;
}

YOCTO_UNIT_TEST_IMPL(primality)
{
    size_t n = 1000;
    if(argc>1)
    {
        n = strconv::to_size(argv[1],"n");
    }

    size_t j0=0;
    for(size_t i=0;i<=n;++i)
    {
        const bool ans = primality::_check(i);
        if(ans != primality::check(i) )
        {
            throw exception("different results for %lu", (unsigned long)(i));
        }


        if(ans)
        {
            std::cerr << ' ' << i;
            if( !(++j0%16) )
            {
                std::cerr << std::endl;
            }
        }

    }
    std::cerr << std::endl;

    for(size_t i=1+alea.leq(10);i>0;--i)
    {
        const size_t j=2+alea.leq(0x10000000);
        std::cerr << primality::prev(j) << "\t<=\t" << j << "\t<=\t" << primality::next(j) << std::endl;
    }


    std::cerr << "[" << j0 << "]" << std::endl;

    if(false)
    {
        std::cerr << "Computing speeds..." << std::endl;
        const double speed0 = check_perf(n,primality::_check);
        std::cerr << "speed0=" << speed0 << std::endl;
        const double speed1 = check_perf(n,primality::check);
        std::cerr << "speed1=" << speed1 << std::endl;
    }
    
}
YOCTO_UNIT_TEST_DONE()

#include "yocto/sequence/vector.hpp"
#include "yocto/ios/ocstream.hpp"
YOCTO_UNIT_TEST_IMPL(primgen)
{
    size_t n = 1000;
    if(argc>1)
    {
        n = strconv::to_size(argv[1],"n");
    }
    fprintf(stderr, "Building List\n");
    vector<size_t> p(65536,as_capacity);

    for(size_t i=5;i*i<=n;i+=6)
    {
        p.push_back(i);
        p.push_back(i+2);
    }
    const size_t N = p.size();
    size_t j=0;
    for(size_t i=1;i<=N;++i)
    {
        //std::cerr << ' ' << p[i];
        fprintf(stderr," %5lu",(unsigned long)(p[i]));
        if( 0 == (++j%16) )
        {
            fprintf(stderr,"\n");
        }
        fflush(stderr);
    }
    std::cerr << std::endl;

    std::cerr << "[" << p.size() << "]" << std::endl;
    std::cerr << "up to " << p.back() << std::endl;
    size_t codeMax = 0;
    j = 0;
    std::cerr.flush();
    for(size_t i=2;i<=N;++i)
    {
        const size_t delta = p[i]-p[i-1];

        fprintf(stderr," %4lu",(unsigned long)(delta));
        if( 0 == (++j%16) )
        {
            fprintf(stderr,"\n");
        }
        fflush(stderr);

        if( 0 != (delta%2) )
        {
            throw exception("invalid delta");
        }
        const size_t half = (delta>>1); assert(half>0);
        const size_t code = half-1;
        if(code>codeMax)
        {
            codeMax = code;
        }

    }
    std::cerr << std::endl;
    std::cerr << "codeMax=" << codeMax << std::endl;
}
YOCTO_UNIT_TEST_DONE()



