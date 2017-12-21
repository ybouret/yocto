#include "yocto/code/isqrt.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/string/conv.hpp"

using namespace yocto;


YOCTO_UNIT_TEST_IMPL(isqrt)
{
    size_t       n = 10;
    if(argc>1)
    {
        n = strconv::to<size_t>(argv[1],"n");
    }
    
    
    std::cerr.flush();
    for(size_t i=0;i<=n;++i)
    {
        const size_t ans = iSquareRoot(i);
        fprintf(stderr,"isqrt(0x%08x)=%u   \r", unsigned(i), unsigned(ans) );
        fflush(stderr);
    }
    fprintf(stderr,"\n\n");
}
YOCTO_UNIT_TEST_DONE()
