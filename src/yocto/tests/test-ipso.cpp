#include "yocto/utest/run.hpp"
#include "yocto/ipso/coord.hpp"

using namespace yocto;
using namespace ipso;

template <typename T,typename COORD>
static inline void display_coords(COORD &C)
{
    const size_t dim = sizeof(COORD)/sizeof(T);
    for(size_t i=0;i<dim;++i)
    {
        std::cerr << coord<T>::get(C,i) << std::endl;
    }
}

YOCTO_UNIT_TEST_IMPL(ipso)
{
    coord<unit_t>::_1 a = 0;
    std::cerr << "a=" << a << std::endl;
    display_coords<unit_t>(a);

    coord<double>::_2 b(1,2);
    std::cerr << "b=" << b << std::endl;
    display_coords<double>(b);

    coord<int>::_3 c(3,4,5);
    std::cerr << "c=" << c << std::endl;
    display_coords<int>(c);

    coord<int>::_3 d = c;
    if( coord<int>::are_same(c,d) )
    {
        std::cerr << "OK" << std::endl;
    }
    d=coord<int>::dec(c);
    std::cerr << "c=" << c << std::endl;
    std::cerr << "d=" << d << std::endl;

}
YOCTO_UNIT_TEST_DONE()


