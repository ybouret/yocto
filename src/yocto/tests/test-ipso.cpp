#include "yocto/utest/run.hpp"
#include "yocto/ipso/split.hpp"
#include "yocto/ipso/field3d.hpp"
#include "yocto/ios/ocstream.hpp"

using namespace yocto;
using namespace ipso;

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

YOCTO_UNIT_TEST_IMPL(ipso)
{
    std::cerr << "COORD" << std::endl;

    coord1D a = 0;
    std::cerr << "a=" << a << std::endl;
    display_coords<unit_t>(a);

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

    {
        std::cerr << std::endl;
        for(size_t cpus=1;cpus<=4;++cpus)
        {
            std::cerr << "#cpus=" << cpus << std::endl;
            split::in1D s(cpus,patch1u);
            for(size_t rank=0;rank<cpus;++rank)
            {
                patch1D sub = s(rank);
                std::cerr << "\trank#" << rank << "=" << sub << std::endl;
            }
            std::cerr << std::endl;
        }
    }


    patch<coord2D> patch2i( coord2D(-5,6), coord2D(5,-6) );
    display_patch(patch2i);
    const coord2D v2(0,0);
    std::cerr << "offset_of(" << v2 << ")=" << patch2i.offset_of(v2) << std::endl;

    {
        std::cerr << std::endl;
        for(size_t cpus=1;cpus<=4;++cpus)
        {
            std::cerr << "#cpus=" << cpus << std::endl;
            for(size_t nx=1;nx<=cpus;++nx)
            {
                for(size_t ny=1;ny<=cpus;++ny)
                {
                    if(nx*ny!=cpus) continue;
                    std::cerr << "\tnx=" << nx << ", ny=" << ny << std::endl;
                    split::in2D s( coord2D(nx,ny),patch2i);
                    for(size_t rank=0;rank<cpus;++rank)
                    {
                        const patch2D sub   = s(rank);
                        std::cerr << "\t\trank#" << rank << "=" << sub << std::endl;
                    }
                    std::cerr << std::endl;
                }
            }
        }
    }

    patch<coord3D> p3( coord3D(1,1,1), coord3D(10,20,30) );
    display_patch(p3);
    {
        std::cerr << std::endl;
        for(size_t cpus=1;cpus<=8;++cpus)
        {
            std::cerr << "#cpus=" << cpus << std::endl;
            for(size_t nx=1;nx<=cpus;++nx)
            {
                for(size_t ny=1;ny<=cpus;++ny)
                {
                    for(size_t nz=1;nz<=cpus;++nz)
                    {
                        if(nx*ny*nz!=cpus) continue;
                        std::cerr << "\tnx=" << nx << ", ny=" << ny << ", nz=" << nz << std::endl;
                        split::in3D s( coord3D(nx,ny,nz),p3);
                        for(size_t rank=0;rank<cpus;++rank)
                        {
                            const patch3D sub   = s(rank);
                            std::cerr << "\t\trank#" << rank << "=" << sub << std::endl;
                        }
                        std::cerr << std::endl;
                    }
                }
            }
        }
    }
    std::cerr << std::endl;


    std::cerr << "FIELDS" << std::endl;

    std::cerr << "\tField1D" << std::endl;
    for(size_t iter=0;iter<100;++iter)
    {
        patch1D p1( alea.leq(100), alea.leq(100) );
        field1D<double> f1f(p1);
        field1D< point2d<int> > f1pi(p1);
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

    std::cerr << "\tField2D" << std::endl;
    for(size_t iter=0;iter<100;++iter)
    {
        patch2D P2( coord2D(alea.leq(100),alea.leq(100)), coord2D(alea.leq(100),alea.leq(100)) );

        field2D<float> f2f(P2);
        field2D< point3d<double> > f2p(P2);
        for(coord1D j=P2.lower.y;j<=P2.upper.y;++j)
        {
            field2D<float>::row &Rj = f2f[j];
            for(coord1D i=P2.lower.x;i<=P2.upper.x;++i)
            {
                Rj[i] = i*j;
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

    std::cerr << "\tField3D" << std::endl;
    for(size_t iter=0;iter<100;++iter)
    {
        patch3D P3( coord3D(alea.leq(100),alea.leq(100),alea.leq(100)), coord3D(alea.leq(100),alea.leq(100),alea.leq(100)) );
        //std::cerr << "P3=" << P3 << std::endl;
        field3D<double> f3d(P3);
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

    std::cerr << "SPLIT" << std::endl;

    {
        patch2D p2( coord2D(1,1), coord2D(10,10) );
        for(size_t cores=1;cores<=4;++cores)
        {
            std::cerr << "\t2D: cores=" << cores << std::endl;
            split::in2D::computeCoresMap(cores,p2.width,coord2D(1,1));
        }

    }

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
}
YOCTO_UNIT_TEST_DONE()


