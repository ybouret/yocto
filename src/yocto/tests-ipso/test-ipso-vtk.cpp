#include "yocto/ipso/vtk.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/math/types.hpp"

using namespace yocto;
using namespace ipso;
using namespace math;

template <typename T> static inline
void fill1D( field1D<T> &F )
{
    const T omega_x = T(2*3.14159265359) / (F.upper-F.lower);

    for(unit_t i=F.lower; i<=F.upper;++i)
    {
        F[i] = Sin( T(i-F.lower) * omega_x );
    }
}

template <typename T> static inline
void fill2D( field2D<T> &F )
{
    const T omega_x = T(2*3.14159265359) / (F.upper.x-F.lower.x);
    const T omega_y = T(2*3.14159265359) / (F.upper.y-F.lower.y);

    for(unit_t j=F.lower.y; j<=F.upper.y;++j)
    {
        const T fy = Sin( T(j-F.lower.y) * omega_y );
        for(unit_t i=F.lower.x; i<=F.upper.x;++i)
        {
            F[j][i] = Sin( T(i-F.lower.x) * omega_x ) * fy;
        }
    }
}


template <typename T> static inline
void fill2D( field2D< point2d<T> > &F )
{
    const T omega_x = T(2*3.14159265359) / (F.upper.x-F.lower.x);
    const T omega_y = T(2*3.14159265359) / (F.upper.y-F.lower.y);

    for(unit_t j=F.lower.y; j<=F.upper.y;++j)
    {
        const T phi_y = T(j-F.lower.y) * omega_y;
        const T sy = Sin(phi_y);
        const T cy = Cos(phi_y);
        for(unit_t i=F.lower.x; i<=F.upper.x;++i)
        {
            const T phi_x = T(i-F.lower.x) * omega_x;
            const T sx    = Sin(phi_x);
            const T cx    =  Cos(phi_x);
            F[j][i].x = sx*cy;
            F[j][i].y = sy*cx;
        }
    }
}


template <typename T> static inline
void fill3D( field3D<T> &F )
{
    const T omega_x = T(2*3.14159265359) / (F.upper.x-F.lower.x);
    const T omega_y = T(2*3.14159265359) / (F.upper.y-F.lower.y);
    const T omega_z = T(1) / (F.upper.z-F.lower.z);

    for(unit_t k=F.lower.z; k<=F.upper.z;++k)
    {
        const T dz = ( T(k-F.lower.z) * omega_z )-T(0.5);
        const T fz = exp( (-dz*dz)*T(4) );
        for(unit_t j=F.lower.y; j<=F.upper.y;++j)
        {
            const T fy = Sin( T(j-F.lower.y) * omega_y );
            for(unit_t i=F.lower.x; i<=F.upper.x;++i)
            {
                F[k][j][i] = fz * Sin( T(i-F.lower.x) * omega_x ) * fy;
            }
        }
    }
}


YOCTO_UNIT_TEST_IMPL(vtk)
{
    if(argc<=1)
    {
        throw exception("usage: %s DIMS", argv[0]);
    }
    const coord3D dims = __coord_parser::get<coord3D>(argv[1],"dims");
    
    {
        const patch1D   p(1,dims.x);
        field1D<float>  A("A",p); fill1D(A);
        field1D<double> B("B",p); fill1D(B);
        ios::wcstream fp("f1d.vtk");
        VTK::InitSaveScalars(fp, "in 1D", A, p);
        VTK::SaveScalars(fp, B, B);
    }
    
    {
        const patch2D              p(coord2D(1,1),dims.xy());
        field2D<float>             A("A",p); fill2D(A);
        field2D<double>            B("B",p); fill2D(B);
        field2D< point2d<float>  > C("C",p); fill2D(C);
        field2D< point2d<double> > D("D",p); fill2D(D);

        ios::wcstream fp("f2d.vtk");
        VTK::InitSaveScalars(fp, "in 2D", A, p);
        VTK::SaveScalars(fp, B, B);
        VTK::SaveScalars(fp, C, p);
        VTK::SaveScalars(fp, D, D);
    }
    
    {
        const patch3D              p(coord3D(1,1,1),dims);
        field3D<float>             A("A",p); fill3D(A);
        field3D<double>            B("B",p); fill3D(B);
        field3D< point3d<float> >  C("C",p);
        field3D< point3d<double> > D("D",p);
        
        ios::wcstream fp("f3d.vtk");
        VTK::InitSaveScalars(fp, "in 2D", A, p);
        VTK::SaveScalars(fp, B, B);
        VTK::SaveScalars(fp, C, p);
        VTK::SaveScalars(fp, D, D);
    }
    
    
    
    
}
YOCTO_UNIT_TEST_DONE()

