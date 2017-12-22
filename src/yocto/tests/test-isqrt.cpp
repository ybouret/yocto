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
    for(uint64_t i=0;i<=uint64_t(n);++i)
    {
        const uint64_t ans = iSquareRoot(i);
        fprintf(stderr,"isqrt(%u)=%u   \r", unsigned(i), unsigned(ans) );
        fflush(stderr);
        //fprintf(stderr,"\n");
        if(ans*ans>i)         throw exception("too high!");
       // if((ans+1)*(ans+1)<i) throw exception("too low!");
    }
    fprintf(stderr,"\n\n");
}
YOCTO_UNIT_TEST_DONE()
