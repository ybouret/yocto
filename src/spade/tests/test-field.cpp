#include "yocto/spade/field2d.hpp"
#include "yocto/spade/ghosts-io.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace spade;
#include <cstring>

namespace
{
    template <typename T>
    static inline
    void check_field1D( field1D<T> &F )
    {
        std::cerr << "Checking Field 1D..." << std::endl;
        std::cerr << "f1d.allocated=" <<  F.allocated << std::endl;
        for(coord1D i=F.outer.lower;i<=F.outer.upper;++i)
        {
            F[i] = static_cast<T>(i);
        }

        for(coord1D i=F.outer.upper;i>=F.outer.lower;--i)
        {
            const T cmp = static_cast<T>(i);
            if(F[i]!=cmp)
            {
                throw exception("invalid F[%d]", int(i));
            }
        }

        ghosts_io gIO(F);


    }

    template <typename T>
    static inline
    void check_field2D( field2D<T> &F )
    {
        std::cerr << "Checking Field 2D..." << std::endl;
        std::cerr << "f2d.allocated=" <<  F.allocated << std::endl;
        std::cerr << "outer: " << F.outer << std::endl;
        std::cerr << "inner: " << F.inner << std::endl;
        T src;
        memset(&src,1,sizeof(T));

        coord2D p;
        for(p.y=F.outer.lower.y;p.y<=F.outer.upper.y;++p.y)
        {
            for(p.x=F.outer.lower.x;p.x<F.outer.upper.x;++p.x)
            {
                F[p.y][p.x] = src;
            }
        }

        ghosts_io gIO(F);


    }


}




YOCTO_UNIT_TEST_IMPL(field)
{
    {
        const layout1D  L(1,10);
#if 1
        const Ghost1D   no_ghost;
        const Ghosts1D  no_ghosts(0,no_ghost,no_ghost);
        field1D<double> A("A",L,no_ghosts);
        check_field1D(A);
#endif

        const Ghost1D   g1(0,1);
        const Ghost1D   g2(0,2);
        const Ghosts1D  gs1(0,g1,g2);

        field1D<uint16_t> B("B",L,gs1);
        check_field1D(B);
    }


    {
        const layout2D L( coord2D(1,1), coord2D(10,5) );
#if 1
        const Ghost2D  no_ghost;
        const Ghosts2D no_ghosts(-1,no_ghost,no_ghost);
        field2D<float> A("A2",L, no_ghosts );
        check_field2D(A);
#endif
        const Ghost2D  glo(coord2D(1,2),coord2D(1,2));
        const Ghost2D  gup(coord2D(3,4),coord2D(1,2));
        const Ghosts2D gs2(0,glo,gup);
        field2D< point2d<float> > B("B2",L, gs2 );
        check_field2D(B);

    }

}
YOCTO_UNIT_TEST_DONE()
