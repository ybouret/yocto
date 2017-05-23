#include "yocto/associative/multi-set.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/string.hpp"
#include "support.hpp"

using namespace yocto;
namespace
{
    static double limit = 0;
    inline bool is_bad_data(double value)
    {
        return value < limit;
    }
}

YOCTO_UNIT_TEST_IMPL(multi_set)
{
    multi_set<string,double> ms;

    for(size_t i=1;i<=4;++i)
    {
        const string k = gen<string>::get();

        for(size_t j=2+alea_leq(10);j>0;--j)
        {
            ms.insert(k,alea<double>());
        }

    }


    std::cerr << ms << std::endl;

    limit=0.5;
    ms.remove_if("hello",is_bad_data);
    std::cerr << ms << std::endl;

    limit=10;
    ms.remove_if("hello",is_bad_data);
    std::cerr << ms << std::endl;


}
YOCTO_UNIT_TEST_DONE()
