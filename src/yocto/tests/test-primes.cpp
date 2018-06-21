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
            throw exception("different result/v1 for %lu", (unsigned long)(i));
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
    std::cerr << "Computing speeds..." << std::endl;
    const double speed0 = check_perf(n,primality::_check);
    std::cerr << "speed0=" << speed0 << std::endl;
    const double speed1 = check_perf(n,primality::check);
    std::cerr << "speed1=" << speed1 << std::endl;

#if 0
    const double speed2 = check_perf(n,primality::check2);
    std::cerr << "speed2=" << speed2 << std::endl;
#endif
    
}
YOCTO_UNIT_TEST_DONE()

#include "yocto/sequence/vector.hpp"
#include "yocto/ios/ocstream.hpp"
YOCTO_UNIT_TEST_IMPL(primgen)
{
    size_t n       = 6542;
    if(argc>1)
    {
        n = strconv::to<size_t>(argv[1],"n");
    }
    size_t i       = 3;
    size_t count   = 0;
    size_t maxCode = 0;
    size_t maxInit = i;
    vector<uint8_t> codes(16384,as_capacity);

    ios::wcstream fp("primgen.dat");
    while(count<n)
    {
        const size_t j = primality::next(i+2);
        const size_t delta = j-i;
        if( 0 != (delta%2)) { throw exception("difference is not even"); }
        const size_t code  = (delta>>1)-1;
        if(code>255)
        {
            break;
        }
        codes.push_back(code);
        if(code>maxCode)
        {
            maxCode = code;
            maxInit = i;
        }
        fp("%u %u %u\n", unsigned(i), unsigned(code), unsigned(maxCode));

        fprintf( stderr, "%10u=>%10u : %4u : @%4u\n", unsigned(i), unsigned(j), unsigned(delta), unsigned(code) );
        i=j;
        ++count;
    }
    fflush(stderr);
    std::cerr << "maxCode=" << maxCode << " for " << maxInit << std::endl;
}
YOCTO_UNIT_TEST_DONE()



