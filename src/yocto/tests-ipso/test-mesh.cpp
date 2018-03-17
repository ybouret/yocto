#include "yocto/ipso/mesh/rectilinear.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace ipso;

YOCTO_UNIT_TEST_IMPL(mesh)
{
    if(argc<=1)
    {
        throw exception("usage: %s DIMS", argv[0]);
    }
    const coord3D dims   = __coord_parser::get<coord3D>(argv[1],"dims");

    std::cerr << "dims   = " << dims   << std::endl;

    {
        axis_names    names("x");
        const patch1D region(1,dims.x);
        rectilinear_mesh<float,1> rmesh( names, region );
        const box<float,1> b(0,1);
        rmesh.map_regular(b,rmesh);

        {
            ios::wcstream fp("rmesh1d.vtk");
            VTK::Header(fp,"rmesh1d");
            rmesh.vtk(fp);
        }

    }

    {
        axis_names    names("x,y");
        const patch2D region(coord2D(1,1),dims.xy());
        rectilinear_mesh<double,2> rmesh( names, region );
        const box<double,2> b( point2d<double>(0,0), point2d<double>(1,1) );
        rmesh.map_regular(b,rmesh);

        {
            ios::wcstream fp("rmesh2d.vtk");
            VTK::Header(fp,"rmesh2d");
            rmesh.vtk(fp);
        }

    }

    {
        axis_names    names("x,y,z");
        const patch3D region(coord3D(1,1,1),dims);
        rectilinear_mesh<double,3> rmesh( names, region );
        const box<double,3> b( point3d<double>(0,0,0), point3d<double>(1,1,1) );
        rmesh.map_regular(b,rmesh);

        {
            ios::wcstream fp("rmesh3d.vtk");
            VTK::Header(fp,"rmesh3d");
            rmesh.vtk(fp);
        }

    }



}
YOCTO_UNIT_TEST_DONE()


