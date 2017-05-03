#include "yocto/spade/field2d.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace spade;

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
    }

    template <typename T>
    static inline
    void check_field2D( field2D<T> &F )
    {
        std::cerr << "Checking Field 2D..." << std::endl;
        std::cerr << "f2d.allocated=" <<  F.allocated << std::endl;

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



    }


}




YOCTO_UNIT_TEST_IMPL(field)
{
    {
        const layout1D  L_A(1,10);
        const ghost1D   no_ghost(0);
        const ghosts1D  no_ghosts(0,no_ghost,no_ghost);
        field1D<double> A(L_A,no_ghosts);
        check_field1D(A);

        const ghost1D   g1(0,1);
        const ghost1D   g2(0,2);
        const ghosts1D  gs1(0,g1,g2);

        field1D<char> B(L_A,gs1);
        check_field1D(B);
    }


    {
        const layout2D L( coord2D(1,1), coord2D(10,5) );
        const ghost2D  no_ghost(0);
        const ghosts2D no_ghosts(0,no_ghost,no_ghost);
        field2D<float> A(L, no_ghosts );
        check_field2D(A);

        const ghost2D  glo(0,coord2D(1,2));
        const ghost2D  gup(0,coord2D(1,2));
        const ghosts2D gs2(0,glo,gup);
        field2D< point2d<float> > B(L, gs2 );
        check_field2D(B);

    }

}
YOCTO_UNIT_TEST_DONE()
