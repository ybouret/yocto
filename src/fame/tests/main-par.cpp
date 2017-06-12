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
        string        info = vformat("ghosts#%u: kind=%s:\n", unsigned(dim), g.kind_text());
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

YOCTO_PROGRAM_START()
{
    YOCTO_MPI_ENV();

    {
        const layout<coord1d> full(1,16);

        {
            MPI.Printf0(stderr,"IS  periodic\n");
            const domain<coord1d> D(MPI.CommWorldRank,MPI.CommWorldSize,NULL,full,1);
            layouts<coord1d>      W(D,ng);
            ghosts_of<coord1d>    G(W);


            MPI.Printf(stderr,"is %2ld.%2ld: layout: inner=[%5ld:%5ld], outer=[%5ld:%5ld]\n",
                       long(D.full.size),long(D.self.rank),
                       long(W.inner.lower),
                       long(W.inner.upper),
                       long(W.outer.lower),
                       long(W.outer.upper)
                       );

            display_ghosts(G);



            //const field1d<char>              Fi("Fi",D,ng);

            const field1d<float>             Ff("Ff",D,ng);

            //const field1d< point3d<double> > Fv3("Fv3",D,ng);



        }


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

            display_ghosts(G);

        }
    }
}
YOCTO_PROGRAM_END()

