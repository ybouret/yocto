#include "yocto/fame/ghost-io.hpp"
#include "yocto/fame/field1d.hpp"
#include "yocto/program.hpp"
#include "yocto/mpi/mpi.hpp"
#include "yocto/container/cslot.hpp"

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
        const ghosts &g = G[dim];
        MPI.Printf(stderr,"ghosts#%u: kind=%s\n", unsigned(dim), g.kind_text());
        if(g.prev)
        {
            MPI.Printf(stderr,"\t+prev@%d\n", int(g.prev->rank));
        }
        else
        {
            MPI.Printf(stderr,"\t!prev\n");
        }

        if(g.next)
        {
            MPI.Printf(stderr,"\t+next@%d\n", int(g.next->rank));
        }
        else
        {
            MPI.Printf(stderr,"\t!next\n");
        }
    }
}

YOCTO_PROGRAM_START()
{
    YOCTO_MPI_ENV();

    const layout<coord1d> full(1,16);

    {
        MPI.Printf0(stderr,"IS  periodic\n");
        const domain<coord1d> D(MPI.CommWorldRank,MPI.CommWorldSize,NULL,full,1);
        layouts<coord1d>      W(D,ng);
        ghosts_of<coord1d>    G(W);

        display_ghosts(G);


        MPI.Printf(stderr,"is %2ld.%2ld: layout: inner=[%5ld:%5ld], outer=[%5ld:%5ld]\n",
                   long(D.full.size),long(D.self.rank),
                   long(W.inner.lower),
                   long(W.inner.upper),
                   long(W.outer.lower),
                   long(W.outer.upper)
                   );



        const field1d<char>              Fi("Fi",D,ng);
        const field1d<float>             Ff("Ff",D,ng);
        const field1d< point3d<double> > Fv3("Fv3",D,ng);



    }

    return 0;

    {
        MPI.Printf0(stderr,"\nNOT  periodic\n");
        const domain<coord1d> D(MPI.CommWorldRank,MPI.CommWorldSize,NULL,full,0);
        layouts<coord1d>      W(D,ng);
        ghosts_of<coord1d>    G(W);



        MPI.Printf(stderr,"is %2ld.%2ld: layout: inner=[%5ld:%5ld], outer=[%5ld:%5ld]\n",
                   long(D.full.size),long(D.self.rank),
                   long(W.inner.lower),
                   long(W.inner.upper),
                   long(W.outer.lower),
                   long(W.outer.upper)
                   );
    }
}
YOCTO_PROGRAM_END()

