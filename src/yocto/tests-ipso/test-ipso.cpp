#include "yocto/utest/run.hpp"
#include "yocto/ipso/field3d.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/ipso/box.hpp"

using namespace yocto;
using namespace ipso;

template <size_t DIM>
struct __vecops
{
    template <typename T>
    static bool eq(const T *lhs, const T *rhs) throw();
};

template <>
struct __vecops<1>
{
    template <typename T>
    static bool eq(const T *lhs, const T *rhs) throw()
    {
        return lhs[0] == rhs[0];
    }
};


template <>
struct __vecops<2>
{
    template <typename T>
    static bool eq(const T *lhs, const T *rhs) throw()
    {
        return (lhs[0] == rhs[0]) && (lhs[1]==rhs[1]);
    }
};

template <>
struct __vecops<3>
{
    template <typename T>
    static bool eq(const T *lhs, const T *rhs) throw()
    {
        return (lhs[0] == rhs[0]) && (lhs[1]==rhs[1]) && (lhs[2]==rhs[2]);
    }
};




template <typename COORD>
bool __coord_are_equal(const COORD &lhs, const COORD &rhs) throw()
{
    return __vecops<sizeof(COORD)/sizeof(coord1D)>::eq( (const coord1D *)&lhs, (const coord1D *)&rhs );
}

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

template <typename BOX>
static inline void display_box( const BOX &b )
{
    std::cerr << "lower   = " << b.lower   << std::endl;
    std::cerr << "upper   = " << b.upper   << std::endl;
    std::cerr << "width   = " << b.width   << std::endl;
    std::cerr << "measure = " << b.measure << std::endl;

}

YOCTO_UNIT_TEST_IMPL(ipso)
{
    std::cerr << "COORD" << std::endl;

    {
        coord1D u=0;
        coord1D v=1;
        if(__coord_are_equal(u,v))
        {
        }
    }

    {
        coord2D u;
        coord2D v;
        if(__coord_are_equal(u,v))
        {

        }
    }

    {
        coord3D u;
        coord3D v;
        if(__coord_are_equal(u,v))
        {

        }
    }


    coord1D a = 0;
    std::cerr << "a=" << a << std::endl;
    display_coords(a);

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

    std::cerr << std::endl;

    std::cerr << "PATCH" << std::endl;

    patch<coord1D> patch1u(10,0);
    display_patch(patch1u);
    std::cerr << "offset_of(5)=" << patch1u.offset_of(5) << std::endl;



    patch<coord2D> patch2i( coord2D(-5,6), coord2D(5,-6) );
    display_patch(patch2i);
    const coord2D v2(0,0);
    std::cerr << "offset_of(" << v2 << ")=" << patch2i.offset_of(v2) << std::endl;



    patch<coord3D> p3( coord3D(1,1,1), coord3D(10,20,30) );
    display_patch(p3);

    std::cerr << std::endl;


    std::cerr << "FIELDS" << std::endl;

    if(true)
    {
        std::cerr << "\tField1D" << std::endl;
        for(size_t iter=0;iter<100;++iter)
        {
            patch1D p1( alea.leq(100), alea.leq(100) );
            field1D<double> f1f("f1f",p1);
            field1D< point2d<int> > f1pi("f1pi",p1);
            for(coord1D i=p1.lower;i<=p1.upper;++i)
            {
                f1f[i]    = i;
                f1pi[i].x = i;
            }
            f1f.ldz();
            f1pi.ldz();

            f1f.ld(1.0f);
            f1pi.ld( point2d<int>(1,2) );
        }
    }

    if(true)
    {
        std::cerr << "\tField2D" << std::endl;
        for(size_t iter=0;iter<100;++iter)
        {
            patch2D P2( coord2D(alea.leq(100),alea.leq(100)), coord2D(alea.leq(100),alea.leq(100)) );

            field2D<float> f2f("f2f",P2);
            field2D< point3d<double> > f2p("f2p",P2);

            for(coord1D j=P2.lower.y;j<=P2.upper.y;++j)
            {
                field2D<float>::row &Rj = f2f[j];
                for(coord1D i=P2.lower.x;i<=P2.upper.x;++i)
                {
                    Rj[i] = float(i*j);
                    f2p[j][i].z = alea.to<double>();
                }
            }

            f2f.ldz();
            f2p.ldz();
            f2f.ld(2.0f);
            f2p.ld( point3d<double>(4,5,6) );

            for(coord1D j=P2.lower.y;j<=P2.upper.y;++j)
            {
                f2f[j].ldz();
                f2p[j].ldz();
                f2f[j].ld(1.0f);
                f2p[j].ld( point3d<double>(1,2,3) );
            }

        }
    }

    std::cerr << "\tField3D" << std::endl;
    for(size_t iter=0;iter<100;++iter)
    {
        patch3D P3( coord3D(alea.leq(100),alea.leq(100),alea.leq(100)), coord3D(alea.leq(100),alea.leq(100),alea.leq(100)) );
        //std::cerr << "P3=" << P3 << std::endl;
        field3D<double> f3d("f3d",P3);
        f3d.ldz();
        f3d.ld(1.0);
        for(coord1D k=P3.lower.z;k<=P3.upper.z;++k)
        {
            f3d[k].ldz();
            f3d[k].ld(k);
            for(coord1D j=P3.lower.y;j<=P3.upper.y;++j)
            {
                f3d[k][j].ldz();
                f3d[k][j].ld(j);
                for(coord1D i=P3.lower.x;i<=P3.upper.x;++i)
                {
                    f3d[k][j][i] = i*j*k;
                }
            }
        }
    }
    std::cerr << std::endl;

    

    {
        ios::ocstream::overwrite("parts.dat");
        for(unsigned nc=1;nc<=128;++nc)
        {
            unsigned count2 = 0;
            unsigned count3 = 0;
            for(unsigned nx=1;nx<=nc;++nx)
            {
                for(unsigned ny=1;ny<=nc;++ny)
                {
                    if(nc==nx*ny)
                    {
                        ++count2;
                    }
                    for(unsigned nz=1;nz<=nc;++nz)
                    {
                        if(nc==nx*ny*nz)
                        {
                            ++count3;
                        }
                    }
                }
            }
            ios::acstream fp("parts.dat");
            fp("%u %u %u\n",nc,count2,count3);
        }
    }

    std::cerr << "Boxes" << std::endl;
    {
        box<float,1> b1f(1,3);
        display_box(b1f);
        box<double,1> b1d(-1,5);
        display_box(b1d);

        box<float,2> b2f( point2d<float>(1,1),  point2d<float>(3,4) );
        display_box(b2f);

        box<double,3> b3d = box<double,3>::from<coord1D,coord1D>( coord3D(0,0,0), coord3D(2,3,4) );
        display_box(b3d);

    }
}
YOCTO_UNIT_TEST_DONE()


