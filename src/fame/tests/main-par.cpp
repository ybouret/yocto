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
static inline void fill2d(  field2d<T> &F )
{
    for(coord1d y=F.outer.lower.y;y<=F.outer.upper.y;++y)
    {
        const bool out_y = (y<F.inner.lower.y||y>F.inner.upper.y);

        for(coord1d x=F.outer.lower.x;x<=F.outer.upper.x;++x)
        {
            if(out_y || (x<F.inner.lower.x||x>F.inner.upper.x) )
            {
                F[y][x] = -(1+F.self.rank);
            }
            else
            {
                assert( F.inner.has( coord2d(x,y) ) );
            }
        }
    }
}

YOCTO_PROGRAM_START()
{
    YOCTO_MPI_ENV();

    {
        const layout<coord1d> full(1,32);
        mpi_ghosts<coord1d>   xch(MPI);
        
        {
            MPI.Printf0(stderr,"IS  periodic\n");
            const domain<coord1d> D(MPI.CommWorldRank,MPI.CommWorldSize,NULL,full,1);
            layouts<coord1d>      W(D,ng);
            ghosts_of<coord1d>    G(W);


            MPI.Printf(stderr,"is %2ld.%2ld: layout: inner=[%5ld:%5ld], outer=[%5ld:%5ld], color=%d\n",
                       long(D.full.size),long(D.self.rank),
                       long(W.inner.lower),
                       long(W.inner.upper),
                       long(W.outer.lower),
                       long(W.outer.upper),
                       int(W.color)
                       );

            display_ghosts(G);




            field1d<float>             Ff("Ff",D,ng);
            Ff.ld(-(MPI.CommWorldRank+1));
            for(coord1d i=Ff.inner.lower;i<=Ff.inner.upper;++i)
            {
                Ff[i] = double(i) / double(full.upper);
            }

            xch.prepare_for(G,Ff);

            {
                const string output = vformat("%d.%d.ini.vtk",MPI.CommWorldSize,MPI.CommWorldRank);
                ios::wcstream fp(output);
                VTK::SaveLayout(fp, "Fields", Ff.outer);
                const string fid = vformat("ff%d.%d",MPI.CommWorldSize,MPI.CommWorldRank);
                VTK::SaveScalars(fp,fid, Ff, Ff.outer);

            }
            xch.perform(G,Ff);
            {
                const string output = vformat("%d.%d.end.vtk",MPI.CommWorldSize,MPI.CommWorldRank);
                ios::wcstream fp(output);
                VTK::SaveLayout(fp, "Fields", Ff.outer);
                const string fid = vformat("ff%d.%d",MPI.CommWorldSize,MPI.CommWorldRank);
                VTK::SaveScalars(fp,fid, Ff, Ff.outer);
            }
        }


        {
            MPI.Printf0(stderr,"\nNOT  periodic\n");
            const domain<coord1d> D(MPI.CommWorldRank,MPI.CommWorldSize,NULL,full,0);
            layouts<coord1d>      W(D,ng);
            ghosts_of<coord1d>    G(W);



            MPI.Printf(stderr,"is %2ld.%2ld: layout: inner=[%5ld:%5ld], outer=[%5ld:%5ld], color=%d\n",
                       long(D.full.size),long(D.self.rank),
                       long(W.inner.lower),
                       long(W.inner.upper),
                       long(W.outer.lower),
                       long(W.outer.upper),
                       int(W.color)
                       );

            display_ghosts(G);
            field1d<double>     Fd("Fd",D,ng);
            xch.prepare_for(G,Fd);

            Fd.ld(-(MPI.CommWorldRank+1));
            for(coord1d i=Fd.inner.lower;i<=Fd.inner.upper;++i)
            {
                Fd[i] = double(i) / double(full.upper);
            }

            {
                const string output = vformat("%d.%d.ini_np.dat",MPI.CommWorldSize,MPI.CommWorldRank);
                ios::wcstream fp(output);
                VTK::SaveLayout(fp, "Fields", Fd.outer);
                const string fid = vformat("fd%d.%d",MPI.CommWorldSize,MPI.CommWorldRank);
                VTK::SaveScalars(fp,fid, Fd, Fd.outer);

            }

            xch.perform(G,Fd);
            {
                const string output = vformat("%d.%d.end_np.dat",MPI.CommWorldSize,MPI.CommWorldRank);
                ios::wcstream fp(output);
                VTK::SaveLayout(fp, "Fields", Fd.outer);
                const string fid = vformat("fd%d.%d",MPI.CommWorldSize,MPI.CommWorldRank);
                VTK::SaveScalars(fp,fid, Fd, Fd.outer);            }
        }
    }

    MPI.Printf0(stderr, "\n\n-------- IN 2D --------\n\n");
    {
        mpi_ghosts<coord2d>   xch(MPI);

        const layout<coord2d> full( coord2d(1,1), coord2d(32,32) );
        {
            MPI.Printf0(stderr,"IS  periodic XY\n");
            const domain<coord2d> D(MPI.CommWorldRank,MPI.CommWorldSize,NULL,full,coord2d(1,1));
            field2d<double>       F("F",D,ng);
            ghosts_of<coord2d>    G(F);
            display_ghosts(G);
            xch.prepare_for(G,F);


            fill2d(F);

            xch.perform(G,F);

        }

        {
            MPI.Printf0(stderr,"IS  periodic X\n");
            const domain<coord2d> D(MPI.CommWorldRank,MPI.CommWorldSize,NULL,full,coord2d(1,0));
            field2d<double>       F("F",D,ng);
            ghosts_of<coord2d>    G(F);
            display_ghosts(G);

            xch.prepare_for(G,F);

            fill2d(F);
            xch.perform(G,F);
            
        }

        {
            MPI.Printf0(stderr,"IS  periodic Y\n");
            const domain<coord2d> D(MPI.CommWorldRank,MPI.CommWorldSize,NULL,full,coord2d(0,1));
            field2d<double>       F("F",D,ng);
            ghosts_of<coord2d>    G(F);
            display_ghosts(G);

            xch.prepare_for(G,F);

            fill2d(F);
            xch.perform(G,F);

        }

        {
            MPI.Printf0(stderr,"IS  NOT periodic\n");
            const domain<coord2d> D(MPI.CommWorldRank,MPI.CommWorldSize,NULL,full,coord2d(0,0));
            field2d<double>       F("F",D,ng);
            ghosts_of<coord2d>    G(F);
            display_ghosts(G);

            xch.prepare_for(G,F);

            fill2d(F);
            xch.perform(G,F);
            
        }



    }

}
YOCTO_PROGRAM_END()

