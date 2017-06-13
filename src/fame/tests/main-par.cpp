#include "yocto/fame/field1d.hpp"
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
                const string output = vformat("%d.%d.ini.dat",MPI.CommWorldSize,MPI.CommWorldRank);
                ios::wcstream fp(output);
                for(coord1d i=Ff.outer.lower;i<=Ff.outer.upper;++i)
                {
                    fp("%g %g\n", double(i), double(Ff[i]));
                }
            }
            xch.perform(G,Ff);
            {
                const string output = vformat("%d.%d.end.dat",MPI.CommWorldSize,MPI.CommWorldRank);
                ios::wcstream fp(output);
                for(coord1d i=Ff.outer.lower;i<=Ff.outer.upper;++i)
                {
                    fp("%g %g\n", double(i), double(Ff[i]));
                }
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
                for(coord1d i=Fd.outer.lower;i<=Fd.outer.upper;++i)
                {
                    fp("%g %g\n", double(i), double(Fd[i]));
                }
            }

            xch.perform(G,Fd);
            {
                const string output = vformat("%d.%d.end_np.dat",MPI.CommWorldSize,MPI.CommWorldRank);
                ios::wcstream fp(output);
                for(coord1d i=Fd.outer.lower;i<=Fd.outer.upper;++i)
                {
                    fp("%g %g\n", double(i), double(Fd[i]));
                }
            }
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

            xch.perform(G,F);

        }
    }

}
YOCTO_PROGRAM_END()

