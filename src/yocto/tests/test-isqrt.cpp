#include "yocto/code/isqrt.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/string/conv.hpp"

using namespace yocto;


YOCTO_UNIT_TEST_IMPL(isqrt)
{
    uint32_t   n = 10;
    if(argc>1)
    {
        n = strconv::to<size_t>(argv[1],"n");
    }
    
    
    std::cerr.flush();
    static const uint64_t K = 100;
    uint64_t old_k = 0;
    for(uint64_t i=0;i<=uint64_t(n);++i)
    {
        const uint64_t ans = iSquareRoot(i);
        const uint64_t k   = (K*i)/n;
        if( (n==i) || (k-old_k>=1) )
        {
            fprintf(stderr,"isqrt(%u)=%u   \r", unsigned(i), unsigned(ans) );
            fflush(stderr);
            old_k = k;
        }
        if(ans*ans>i)         throw exception("%lu: too high!", (unsigned long)i);
        if((ans+1)*(ans+1)<i) throw exception("%lu: too low!",  (unsigned long)i);
    }
    fprintf(stderr,"\n\n");
}
YOCTO_UNIT_TEST_DONE()

