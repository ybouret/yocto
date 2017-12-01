#include "yocto/utest/run.hpp"
#include "yocto/ipso/split.hpp"

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

    {
        std::cerr << std::endl;
        for(size_t cpus=1;cpus<=4;++cpus)
        {
            std::cerr << "#cpus=" << cpus << std::endl;
            split::in1D s(cpus,patch1u);
            for(size_t rank=0;rank<cpus;++rank)
            {
                patch1D sub = s(rank);
                std::cerr << "\trank#" << rank << "=" << sub << std::endl;
            }
            std::cerr << std::endl;
        }
    }


    patch<coord2D> patch2i( coord2D(-5,6), coord2D(5,-6) );
    display_patch(patch2i);
    const coord2D v2(0,0);
    std::cerr << "offset_of(" << v2 << ")=" << patch2i.offset_of(v2) << std::endl;

    {
        std::cerr << std::endl;
        for(size_t cpus=1;cpus<=4;++cpus)
        {
            std::cerr << "#cpus=" << cpus << std::endl;
            for(size_t nx=1;nx<=cpus;++nx)
            {
                for(size_t ny=1;ny<=cpus;++ny)
                {
                    if(nx*ny!=cpus) continue;
                    std::cerr << "\tnx=" << nx << ", ny=" << ny << std::endl;
                    split::in2D s(nx,ny,patch2i);
                    for(size_t rank=0;rank<cpus;++rank)
                    {
                        const coord2D ranks = s.get_ranks(rank);
                        std::cerr << "\t\tranks(" << rank << ")=" << ranks << std::endl;
                        const patch2D sub   = s(rank);
                        std::cerr << "\t\trank#" << rank << "=" << sub << std::endl;
                    }
                    std::cerr << std::endl;
                }
            }
        }
    }

}
YOCTO_UNIT_TEST_DONE()


