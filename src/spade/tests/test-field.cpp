#include "yocto/spade/field3d.hpp"
#include "yocto/spade/ghosts-io.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace spade;
#include <cstring>

namespace
{
    template <typename T>
    static inline
    void check_field1D( Field1D<T> &F )
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
    void check_field2D( Field2D<T> &F )
    {
        std::cerr << "Checking Field 2D..." << std::endl;
        std::cerr << "f2d.allocated=" <<  F.allocated << std::endl;
        std::cerr << "outer: " << F.outer << std::endl;
        std::cerr << "inner: " << F.inner << std::endl;
        T src;
        memset(&src,1,sizeof(T));
        std::cerr << "\tchecking IO" << std::endl;
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

    template <typename T>
    static inline
    void check_field3D( Field3D<T> &F )
    {
        std::cerr << "Checking Field 3D..." << std::endl;
        std::cerr << "f3d.allocated=" <<  F.allocated << std::endl;
        std::cerr << "outer: " << F.outer << std::endl;
        std::cerr << "inner: " << F.inner << std::endl;
        T src;
        memset(&src,1,sizeof(T));
        std::cerr << "\tchecking IO" << std::endl;
        coord3D p;
        for(p.z=F.outer.lower.z;p.z<=F.outer.upper.z;++p.z)
        {
            for(p.y=F.outer.lower.y;p.y<=F.outer.upper.y;++p.y)
            {
                for(p.x=F.outer.lower.x;p.x<F.outer.upper.x;++p.x)
                {
                    F[p.z][p.y][p.x] = src;
                }
            }
        }
        //ghosts_io gIO(F);


    }

}




YOCTO_UNIT_TEST_IMPL(field)
{
    {
        const Layout1D  L(1,10);
#if 1
        const Ghost1D   no_ghost;
        const Ghosts1D  no_ghosts(0,no_ghost,no_ghost);
        Field1D<double> A("A",L,no_ghosts);
        check_field1D(A);
#endif

        const Ghost1D   g1(0,1);
        const Ghost1D   g2(0,2);
        const Ghosts1D  gs1(0,g1,g2);

        Field1D<uint16_t> B("B",L,gs1);
        check_field1D(B);
    }


    {
        const Layout2D L( coord2D(1,1), coord2D(10,5) );
#if 1
        const Ghost2D  no_ghost;
        const Ghosts2D no_ghosts(-1,no_ghost,no_ghost);
        Field2D<float> A("A2",L, no_ghosts );
        check_field2D(A);
#endif
        const Ghost2D  glo(coord2D(1,2),coord2D(1,2));
        const Ghost2D  gup(coord2D(3,4),coord2D(1,2));
        const Ghosts2D gs2(0,glo,gup);
        Field2D< point2d<float> > B("B2",L, gs2 );
        check_field2D(B);

    }

    {
        const Layout3D L( coord3D(1,1,1), coord3D(5,6,8) );
        const Ghost3D  glo( coord3D(10,11,12), coord3D(1,2,3) );
        const Ghost3D  gup( coord3D(13,14,15), coord3D(3,2,1) );
        const Ghosts3D gs3(0,glo,gup);
        
        Field3D< double > A("A3",L,gs3);
        check_field3D(A);
    }
    
    
}
YOCTO_UNIT_TEST_DONE()