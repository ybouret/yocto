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
    for(size_t i=0;i<=n;++i)
    {
        const size_t ans = iSquareRoot(i);
        std::cerr << "sqrt(" << i << "/" << n << ")=" << ans << std::endl;
    }
}
YOCTO_UNIT_TEST_DONE()
