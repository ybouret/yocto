#include "yocto/utest/run.hpp"
#include "yocto/ipso/patch.hpp"
#include "yocto/ipso/utils.hpp"

using namespace yocto;
using namespace ipso;

template <typename COORD>
static inline void display_coords(COORD &C)
{
    const size_t dim = sizeof(COORD)/sizeof(coord1D);
    for(size_t i=0;i<dim;++i)
    {
        std::cerr << __coord(C,i) << std::endl;
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
    coord1D a = 0;
    std::cerr << "a=" << a << std::endl;
    display_coords<unit_t>(a);

    coord2D b(1,2);
    std::cerr << "b=" << b << std::endl;
    display_coords(b);

    coord3D c(3,4,5);
    std::cerr << "c=" << c << std::endl;
    display_coords(c);

    coord3D d = c;
    if( __coord_eq(c,d) )
    {
        std::cerr << "OK" << std::endl;
    }


    patch<coord1D> patch1u(10,0);
    display_patch(patch1u);
    std::cerr << "offset_of(5)=" << patch1u.offset_of(5) << std::endl;


    patch<coord2D> patch2i( coord2D(-5,6), coord2D(5,-6) );
    display_patch(patch2i);
    const coord2D v2(0,0);
    std::cerr << "offset_of(" << v2 << ")=" << patch2i.offset_of(v2) << std::endl;

}
YOCTO_UNIT_TEST_DONE()


