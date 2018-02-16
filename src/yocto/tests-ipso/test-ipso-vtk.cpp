#include "yocto/ipso/vtk.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ios/ocstream.hpp"

using namespace yocto;
using namespace ipso;

YOCTO_UNIT_TEST_IMPL(vtk)
{
    if(argc<=1)
    {
        throw exception("usage: %s DIMS", argv[0]);
    }
    const coord3D dims = __coord_parser::get<coord3D>(argv[1],"dims");
    
    {
        const patch1D p(1,dims.x);
        const field1D<float>  A("A",p);
        const field1D<double> B("B",p);
        ios::wcstream fp("f1d.vtk");
        VTK::InitSaveScalars(fp, "in 1D", A, p);
        VTK::SaveScalars(fp, B, B);
    }
    
    {
        const patch2D p(coord2D(1,1),dims.xy());
        const field2D<float>             A("A",p);
        const field2D<double>            B("B",p);
        const field2D< point2d<float> >  C("C",p);
        const field2D< point2d<double> > D("D",p);

        ios::wcstream fp("f2d.vtk");
        VTK::InitSaveScalars(fp, "in 2D", A, p);
        VTK::SaveScalars(fp, B, B);
        VTK::SaveScalars(fp, C, p);
        VTK::SaveScalars(fp, D, D);
    }
    
    {
        const patch3D p(coord3D(1,1,1),dims);
        const field3D<float>             A("A",p);
        const field3D<double>            B("B",p);
        const field3D< point3d<float> >  C("C",p);
        const field3D< point3d<double> > D("D",p);
        
        ios::wcstream fp("f3d.vtk");
        VTK::InitSaveScalars(fp, "in 2D", A, p);
        VTK::SaveScalars(fp, B, B);
        VTK::SaveScalars(fp, C, p);
        VTK::SaveScalars(fp, D, D);
    }
    
    
    
    
}
YOCTO_UNIT_TEST_DONE()

