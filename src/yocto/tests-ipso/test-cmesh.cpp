
#include "yocto/ipso/mesh/curvilinear.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace ipso;

YOCTO_UNIT_TEST_IMPL(cmesh)
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
        _curvilinear_mesh<float,1> cmesh( names, region );
        const box<float,1> b(0,1);
        cmesh.map_regular(b,cmesh);

        {
            ios::wcstream fp("cmesh1d.vtk");
            VTK::Header(fp,"cmesh1d");
            cmesh.vtk(fp);
        }
    }

    {
        axis_names    names("x,y");
        const patch2D region(coord2D(1,1),dims.xy());
        _curvilinear_mesh<double,2> cmesh( names, region );
        const box<double,2> b( point2d<double>(0,0), point2d<double>(1,1) );
        cmesh.map_regular(b,cmesh);

        {
            ios::wcstream fp("cmesh2d.vtk");
            VTK::Header(fp,"cmesh2d");
            cmesh.vtk(fp);
        }
    }

    {
        axis_names    names("x,y,z");
        const patch3D region(coord3D(1,1,1),dims);
        _curvilinear_mesh<double,3> cmesh( names, region );
        const box<double,3> b( point3d<double>(0,0,0), point3d<double>(1,1,1) );
        cmesh.map_regular(b,cmesh);

        {
            ios::wcstream fp("cmesh3d.vtk");
            VTK::Header(fp,"cmesh3d");
            cmesh.vtk(fp);
        }
    }

}
YOCTO_UNIT_TEST_DONE()

