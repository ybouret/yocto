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
            throw exception("different results for %lu, level-1", (unsigned long)(i));
        }

#if 0
        if(ans != primality::check2(i) )
        {
            throw exception("different results for %lu, level-2", (unsigned long)(i));
        }
#endif

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
        //const double speed2 = check_perf(n,primality::check2);
        //std::cerr << "speed2=" << speed2 << std::endl;
    }
    
}
YOCTO_UNIT_TEST_DONE()

#include "yocto/sequence/vector.hpp"
#include "yocto/ios/ocstream.hpp"
YOCTO_UNIT_TEST_IMPL(primgen)
{

    const uint64_t n = limit_of<uint32_t>::maximum;
    const uint64_t m = limit_of<uint16_t>::maximum;

    vector<uint16_t> p(32768,as_capacity);
    uint64_t i = 5;
    for(;i*i<=n;i+=6)
    {
        if(i>m)
        {
            throw exception("primgen failure");
        }
        p.push_back(uint16_t(i));
        p.push_back(uint16_t(i+2));
    }
    std::cerr << "[" << p.size() << "]" << std::endl;
    const unsigned N = p.size();
    ios::wcstream fp("prime32.inc");
    fp("static const size_t   __prime32_idx = 0x%lx;\n", (unsigned long)(i) );
    fp("static const uint16_t __prime32[%u]={\n",N);
    size_t j=0;
    for(unsigned i=1;i<=N;++i)
    {
        fp(" 0x%04x", p[i]);
        if(i<N) fp << ',';
        if( 0 == (++j%16) ) fp << '\n';
    }
    fp("\n};\n");
    fp("static const size_t  __prime32_num = sizeof(__prime32)/sizeof(__prime32[0]);\n");


#if 0
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
#endif

}
YOCTO_UNIT_TEST_DONE()



