#include "yocto/utest/run.hpp"
#include "yocto/ipso/patch.hpp"

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

template <typename PATCH>
static inline void display_patch(const PATCH &p)
{
    std::cerr << "lower=" << p.lower << std::endl;
    std::cerr << "upper=" << p.upper << std::endl;
    std::cerr << "width=" << p.width << std::endl;
    std::cerr << "pitch=" << p.pitch << std::endl;
    std::cerr << "items=" << p.items << std::endl;
    std::cerr << "patch=" << p << std::endl;
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
    if( coord<int>::eq(c,d) )
    {
        std::cerr << "OK" << std::endl;
    }
    d=coord<int>::dec(c);
    std::cerr << "c=" << c << std::endl;
    std::cerr << "d=" << d << std::endl;

    patch<unsigned,coord<unsigned>::_1> patch1u(10,0);
    display_patch(patch1u);
    std::cerr << "offset_of(5)=" << patch1u.offset_of(5) << std::endl;


    patch<unit_t,coord<unit_t>::_2> patch2i( coord<unit_t>::_2(-5,6), coord<unit_t>::_2(5,-6) );
    display_patch(patch2i);
    const coord<unit_t>::_2 v2(0,0);
    std::cerr << "offset_of(" << v2 << ")=" << patch2i.offset_of(v2) << std::endl;

}
YOCTO_UNIT_TEST_DONE()


