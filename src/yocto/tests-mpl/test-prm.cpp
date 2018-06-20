#include "yocto/mpl/natural.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/string/conv.hpp"
#include "yocto/code/primality.hpp"

using namespace yocto;

YOCTO_UNIT_TEST_IMPL(prm)
{
    uint32_t n = 100;
    if(argc>1)
    {
        n = strconv::to<size_t>(argv[1],"n");
    }

    size_t count=0;
    for(uint32_t i=0;i<=n;++i)
    {
        const bool ans = primality::check(i);
        if(ans)
        {
            ++count;
        }
        const mpn j = i;
        if( ans != mpn::__is_prime(j) )
        {
            throw exception("primality failure for %lu", (unsigned long)i );
        }
        if(ans)
        {
            std::cout << ' ' << i;
            if( (0==(count%16)) )
            {
                std::cout << std::endl;
            }
        }
    }
    std::cout << std::endl;
    std::cout << '[' << count << ']' << std::endl;

}
YOCTO_UNIT_TEST_DONE()

