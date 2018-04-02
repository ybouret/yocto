
#include "yocto/ipso/mesh/point.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/string/conv.hpp"

using namespace yocto;
using namespace ipso;
#if 0

YOCTO_UNIT_TEST_IMPL(pmesh)
{
    if(argc<=1)
    {
        throw exception("usage: %s size", argv[0]);
    }

    const size_t sz = strconv::to_size(argv[1],"size");
    if(sz<=1) throw exception("I need size>1");
    patch1D region(1,sz);

    axis_names an1("x");
    point_mesh<float,1> p1(an1,region);

    axis_names an2("x,y");
    point_mesh<double,2> p2(an2,region);

    axis_names an3("x,y,z");
    point_mesh<float,3> p3(an3,region);

    for(size_t i=1;i<=sz;++i)
    {
        const float theta = ((i-1) * 6.28f) / (sz-1);
        p2.X()[i] = p1.X()[i] = p3.X()[i] = cosf(theta);
        p2.Y()[i] = p3.Y()[i] = sinf(theta);
        p3.Z()[i] = theta/3;
    }


    {
        ios::wcstream fp("pmesh1d.vtk");
        VTK::Header(fp, "pmesh1d");
        p1.vtk(fp);
    }

    {
        ios::wcstream fp("pmesh2d.vtk");
        VTK::Header(fp, "pmesh2d");
        p2.vtk(fp);
    }

    {
        ios::wcstream fp("pmesh3d.vtk");
        VTK::Header(fp, "pmesh3d");
        p3.vtk(fp);
    }

}
YOCTO_UNIT_TEST_DONE()
#endif

