#include "yocto/utest/run.hpp"
#include "yocto/code/primes.hpp"
#include "yocto/code/alea.hpp"

using namespace yocto;

YOCTO_UNIT_TEST_IMPL(primes)
{
    size_t p=0;
    for( size_t i=0; i < 25; ++i )
    {
        for( size_t j=0; j < 16; ++j, ++p )
        {
            std::cerr << (p=next_prime(p)) << " ";
        }
        std::cerr << std::endl;
    }

    for( size_t i=0; i < 10; ++i )
    {
        for( size_t j=0; j < 4; ++j )
        {
            const size_t n = 2 + alea.lt( 0xFFFFFF );
            std::cerr << "(" << prev_prime(n) << "," << n << "," << next_prime(n) << ") ";
        }
        std::cerr << std::endl;
    }
}
YOCTO_UNIT_TEST_DONE()


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
        const bool ans = primality::check0(i);
        if(ans != primality::check1(i) )
        {
            throw exception("different result for %lu", (unsigned long)(n));
        }
        if(ans)
        {
            //std::cerr << ' ' << i;
            if( !(++j0%16) )
            {
                //std::cerr << std::endl;
            }
        }

    }
    //std::cerr << std::endl;
    std::cerr << "[" << j0 << "]" << std::endl;
    std::cerr << "Computing speeds..." << std::endl;
    const double speed0 = check_perf(n,primality::check0);
    std::cerr << "speed0=" << speed0 << std::endl;
    const double speed1 = check_perf(n,primality::check1);
    std::cerr << "speed1=" << speed1 << std::endl;
}
YOCTO_UNIT_TEST_DONE()

#include "yocto/sequence/vector.hpp"
#include "yocto/code/binary.hpp"
#include "yocto/code/utils.hpp"
#include "yocto/ios/ocstream.hpp"

YOCTO_UNIT_TEST_IMPL(genprimes)
{
    size_t n = 16384;
    if(argc>1)
    {
        n = strconv::to_size(argv[1],"n");
    }
    vector<size_t> codes(n,as_capacity);

    size_t p=3;
    size_t q=5;
    size_t max_bits=0;
    size_t max_bytes=0;
    do
    {
        const size_t d = q-p; assert(0==( (q-p)%2 ));
        const size_t code = (d/2)-1;
        const size_t code_bytes = bytes_for(code);
        const size_t code_bits  = bits_for(code);
        max_bits  = max_of(max_bits,code_bits);
        max_bytes = max_of(max_bytes,code_bytes);
        if(code_bytes>1)
        {
            fprintf(stderr,"reached max encoding length\n");
            break;
        }
        codes.push_back(code);
        //std::cerr << "d=" << d << " => " << code << std::endl;
        fprintf(stderr,"%12u->%12u : d=%4u => %4u | %2u (%2u)\n", unsigned(p), unsigned(q), unsigned(d), unsigned(code), unsigned(code_bytes), unsigned(code_bits) );
        p=q;

        /*
        if(p>=65535)
        {
            fprintf(stderr,"reached 32bits limit\n");
            break;
        }
         */

        do
        {
            q += 2;
        } while( !primality::check0(q) );

    } while(codes.size()<n);
    fprintf(stderr,"#codes=%u | max_bits=%2u | max_bytes=%2u\n", unsigned(codes.size()), unsigned(max_bits), unsigned(max_bytes) );

    ios::wcstream fp("prmcodes.inc");

}
YOCTO_UNIT_TEST_DONE()
