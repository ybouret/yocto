#include "yocto/spade/mesh/rectilinear.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace spade;

YOCTO_UNIT_TEST_IMPL(rmesh)
{
    {
        const Layout1D L(-5,8);
        const Ghost1D  lower_ghost(-10,1);
        const Ghost1D  upper_ghost(10,2);
        const Ghosts1D G(-1,lower_ghost,upper_ghost);

        typedef vertex_for<double,coord1D>::type vtx1D;
        box_of<double,coord1D> B( vtx1D(-1), vtx1D(1) );

        std::cerr << "B1D=" << B << std::endl;
        RectilinearMesh<double,coord1D> rm("mesh1d",L,G,&B);

        const Field1D<double> &X = rm[0];
        for(coord1D i=X.outer.lower;i<=X.outer.upper;++i)
        {
            std::cerr << "X[" << i << "]=" << X[i] << std::endl;
        }
        std::cerr << std::endl;
    }

    {
        const Layout2D L( coord2D(1,1), coord2D(10,5)  );
        const Ghost2D  g_lo(coord2D(11,12), coord2D(1,2) );
        const Ghost2D  g_up(coord2D(13,14), coord2D(1,2) );
        const Ghosts2D G(0,g_lo,g_up);

        typedef vertex_for<float,coord2D>::type vtx2D;
        box_of<float,coord2D> B( vtx2D(0,0), vtx2D(1,1) );
        std::cerr << "B2D=" << B << std::endl;

        RectilinearMesh<float,coord2D> rm("mesh2d",L,G,&B);

        for(size_t dim=0;dim<2;++dim)
        {
            Field1D<float> &U = rm[dim];
            for(coord1D i=U.outer.lower;i<=U.outer.upper;++i)
            {
                std::cerr << coord_data::axis_name(dim) << "[" << i << "]=" << U[i] << std::endl;
            }
            std::cerr << std::endl;
        }

    }


    {
        const Layout3D L( coord3D(1,1,1), coord3D(10,15,20)  );
        const Ghost3D  g_lo(coord3D(11,12,13), coord3D(1,2,3) );
        const Ghost3D  g_up(coord3D(14,15,16), coord3D(1,2,3) );
        const Ghosts3D G(0,g_lo,g_up);

        typedef vertex_for<long double,coord3D>::type vtx3D;
        box_of<long double,coord3D> B( vtx3D(0,0,0), vtx3D(1,2,3) );
        std::cerr << "B3D=" << B << std::endl;

        RectilinearMesh<long double,coord3D> rm("mesh2d",L,G,&B);

        for(size_t dim=0;dim<3;++dim)
        {
            Field1D<long double> &U = rm[dim];
            for(coord1D i=U.outer.lower;i<=U.outer.upper;++i)
            {
                std::cerr << coord_data::axis_name(dim) << "[" << i << "]=" << U[i] << std::endl;
            }
            std::cerr << std::endl;
        }

    }
}
YOCTO_UNIT_TEST_DONE()

