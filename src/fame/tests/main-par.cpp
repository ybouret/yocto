#include "yocto/fame/format/vtk.hpp"
#include "yocto/program.hpp"
#include "yocto/fame/mpi.hpp"

using namespace yocto;
using namespace fame;


static size_t ng = 2;

template <typename COORD>
static inline
void display_ghosts(const ghosts_of<COORD> &G )
{
    YOCTO_MPI_GET();
    for(size_t dim=0;dim<YOCTO_FAME_DIM_OF(COORD);++dim)
    {
        const ghosts &g    = G[dim];
        string        info = vformat("ghosts#%u: kind=%s:\n", unsigned(dim), g.kind_text() );
        if(g.prev)
        {
            info  += vformat("\t+prev@%d\n", int(g.prev->rank));
        }
        else
        {
            info  += vformat("\t!prev\n");
        }

        if(g.next)
        {
            info  += vformat("\t+next@%d\n", int(g.next->rank));
        }
        else
        {
            info  += vformat("\t!next\n");
        }
        MPI.Printf(stderr,"%s",info.c_str());
    }
}

#include "yocto/ios/ocstream.hpp"


template <typename T>
static inline void fill2d(  field2d<T> &F, const layout<coord2d> &full)
{
    for(coord1d y=F.outer.lower.y;y<=F.outer.upper.y;++y)
    {
        const bool out_y = (y<F.inner.lower.y||y>F.inner.upper.y);
        const double Y = ( double(y)-double(full.lower.y) ) / double(full.width.y-1);
        for(coord1d x=F.outer.lower.x;x<=F.outer.upper.x;++x)
        {
            const double X = ( double(x) - double(full.lower.x) ) / double(full.width.x-1);
            if(out_y || (x<F.inner.lower.x||x>F.inner.upper.x) )
            {
                F[y][x] = -(1+F.self.rank);
            }
            else
            {
                assert( F.inner.has( coord2d(x,y) ) );
                F[y][x] = T(cos(6*X)+sin(6*Y));
            }
        }
    }
}

template <typename T>
static inline
void fill3d( field3d<T> &F, const layout<coord3d> &full )
{
    F.ld( -(1+F.self.rank) );
    for(coord1d z=F.inner.lower.z;z<=F.inner.upper.z;++z)
    {
        const double dZ = (z-full.lower.z)/double(full.width.z-1) - 0.5;
        for(coord1d y=F.inner.lower.y;y<=F.inner.upper.y;++y)
        {
            const double dY = (y-full.lower.y)/double(full.width.y-1) - 0.5;
            for(coord1d x=F.inner.lower.x;x<=F.inner.upper.x;++x)
            {
                const double dX = (x-full.lower.x)/double(full.width.x-1) - 0.5;
                F[z][y][x] = exp(-0.5*( square_of(dX) + square_of(dY) + square_of(dZ) ));
            }
        }
    }
}

static inline
void demo1d(const mpi             &MPI,
            const layout<coord1d> &full,
            const coord1d          pbc)
{
    mpi_ghosts<coord1d>   mpi_xch(MPI);
    MPI.Printf0(stderr,"pbc=%d\n",int(pbc));
    string prefix = "1d_";
    if(pbc)
    {
        prefix << "x";
    }
    else
    {
        prefix << "no";
    }

    prefix << "_";

    const mpi_domains<coord1d> doms(MPI,NULL,full,pbc);
    const domain<coord1d>     &D = doms[MPI.CommWorldRank];
    fields iof;
    field1d<float>     &F1 = iof.record( new field1d<float >("F1",D,ng)  );
    field1d<double>    &F2 = iof.record( new field1d<double>("F2",D,ng)  );
    field1d<short>     &F3 = iof.record( new field1d<short >("F3",D,ng)  );
    ghosts_of<coord1d>        G(F1);
    F1.ld((MPI.CommWorldRank+1));
    F2.ld((MPI.CommWorldRank+1));
    F3.ld(MPI.CommWorldRank+1);


    mpi_xch.prepare_for(G,iof);
    field1d<float> all1( "all1", doms.io_domain() );

    {
        const string  output = MPI.VTK_FileName(prefix);
        ios::wcstream fp(output);
        VTK::InitSaveScalars(fp, "field1d", F1.name + "_ini", F1, F1.outer);
        mpi_xch.perform(G,iof);
        mpi_xch.perform(G,F2);
        mpi_xch.perform(G,F3);
        VTK::SaveScalars(fp, F1.name + "_end",F1,F1.outer);
    }

    doms.collect(all1,F1);
    if(MPI.IsFirst)
    {
        const string output = prefix + "full.vtk";
        ios::wcstream fp(output);
        VTK::InitSaveScalars(fp,"field1d-all",all1.name,all1,all1.inner);
        all1.ld(1);
    }

    doms.dispatch(all1,F1);
}

static inline
void demo2d(const mpi             &MPI,
            const layout<coord2d> &full,
            const coord2d          pbc)
{
    mpi_ghosts<coord2d>   mpi_xch(MPI);
    MPI.Printf0(stderr,"pbc=%d.%d\n",int(pbc.x),int(pbc.y));
    string prefix = "2d_";
    if(pbc.x)
    {
        prefix << "x";
    }

    if(pbc.y)
    {
        prefix << "y";
    }

    prefix << "_";

    const mpi_domains<coord2d> doms(MPI,NULL,full,pbc);
    const domain<coord2d>     &D = doms[MPI.CommWorldRank];
    fields iof;
    field2d<float>     &F1 = iof.record( new field2d<float >("F1",D,ng)  );
    field2d<double>    &F2 = iof.record( new field2d<double>("F2",D,ng)  );
    field2d<short>     &F3 = iof.record( new field2d<short >("F3",D,ng)  );
    ghosts_of<coord2d>        G(F1);
    F1.ld((MPI.CommWorldRank+1));
    F2.ld((MPI.CommWorldRank+1));
    F3.ld(MPI.CommWorldRank+1);
    fill2d(F1,full);
    fill2d(F2,full);

    mpi_xch.prepare_for(G,iof);
    field2d<float> all1( "all1", doms.io_domain() );

    {
        const string  output = MPI.VTK_FileName(prefix);
        ios::wcstream fp(output);
        VTK::InitSaveScalars(fp, "field2d", F1.name + "_ini", F1, F1.outer);
        mpi_xch.perform(G,iof);
        mpi_xch.perform(G,F2);
        mpi_xch.perform(G,F3);
        VTK::SaveScalars(fp, F1.name + "_end",F1,F1.outer);
    }

    doms.collect(all1,F1);
    if(MPI.IsFirst)
    {
        const string output = prefix + "full.vtk";
        ios::wcstream fp(output);
        VTK::InitSaveScalars(fp,"field3d-all",all1.name,all1,all1.inner);
        all1.ld(1);
    }
    doms.dispatch(all1,F1);

    
}


static inline
void demo3d(const mpi             &MPI,
            const layout<coord3d> &full,
            const coord3d          pbc)
{
    mpi_ghosts<coord3d>   mpi_xch(MPI);
    MPI.Printf0(stderr,"pbc=%d.%d.%d\n",int(pbc.x),int(pbc.y),int(pbc.z));
    string prefix = "3d_";
    if(pbc.x)
    {
        prefix << "x";
    }

    if(pbc.y)
    {
        prefix << "y";
    }

    if(pbc.z)
    {
        prefix << "z";
    }

    prefix << "_";

    const mpi_domains<coord3d> doms(MPI,NULL,full,pbc);
    const domain<coord3d>     &D = doms[MPI.CommWorldRank];
    fields iof;
    field3d<float>     &F1 = iof.record( new field3d<float >("F1",D,ng)  );
    field3d<double>    &F2 = iof.record( new field3d<double>("F2",D,ng)  );
    field3d<short>     &F3 = iof.record( new field3d<short >("F3",D,ng)  );
    ghosts_of<coord3d>        G(F1);
    F1.ld((MPI.CommWorldRank+1));
    F2.ld((MPI.CommWorldRank+1));
    F3.ld(MPI.CommWorldRank+1);
    fill3d(F1,full);
    fill3d(F2,full);

    mpi_xch.prepare_for(G,iof);
    field3d<float> all1( "all1", doms.io_domain() );

    {
        const string  output = MPI.VTK_FileName(prefix);
        ios::wcstream fp(output);
        VTK::InitSaveScalars(fp, "field3d", F1.name + "_ini", F1, F1.outer);
        mpi_xch.perform(G,iof);
        mpi_xch.perform(G,F2);
        mpi_xch.perform(G,F3);
        VTK::SaveScalars(fp, F1.name + "_end",F1,F1.outer);
    }

    doms.collect(all1,F1);
    if(MPI.IsFirst)
    {
        const string output = prefix + "full.vtk";
        ios::wcstream fp(output);
        VTK::InitSaveScalars(fp,"field3d-all",all1.name,all1,all1.inner);
        all1.ld(1);
    }
    doms.dispatch(all1,F1);
    
    
}



#include "yocto/string/conv.hpp"

YOCTO_PROGRAM_START()
{
    YOCTO_MPI_ENV();

    int nmax = 32;
    if(argc>1)
    {
        nmax = strconv::to_int(argv[1],"nmax");
    }




    if(true)
    {
        MPI.Printf0(stderr, "\n\n-------- IN 1D --------\n\n");
        const layout<coord1d> full(1,nmax);
        if(true)
        {
            demo1d(MPI, full,1);
        }

        if(true)
        {
            demo1d(MPI,full,0);
        }

    }


    if(true)
    {
        MPI.Printf0(stderr, "\n\n-------- IN 2D --------\n\n");
        const layout<coord2d> full(coord2d(1,1),coord2d(nmax,nmax));

        if(true)
        {
            demo2d(MPI, full,coord2d(1,1));
        }

        if(true)
        {
            demo2d(MPI,full,coord2d(1,0));
        }

        if(true)
        {
            demo2d(MPI,full,coord2d(0,1));
        }

    }
    
    if(true)
    {
        MPI.Printf0(stderr, "\n\n-------- IN 3D --------\n\n");
        const layout<coord3d> full(coord3d(1,1,1),coord3d(nmax,nmax,nmax));

        if(true)
        {
            demo3d(MPI, full,coord3d(1,1,1));
        }


    }
    
}
YOCTO_PROGRAM_END()

