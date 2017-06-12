#include "yocto/fame/ghost-io.hpp"
#include "yocto/fame/field1d.hpp"
#include "yocto/program.hpp"
#include "yocto/mpi/mpi.hpp"
#include "yocto/container/cslot.hpp"

using namespace yocto;
using namespace fame;


static size_t ng = 2;

template <typename FIELD>
static inline
void test_field_io(const ghost &g, const FIELD &F )
{
    const size_t gs  = g.size();
    const size_t bpp = F.item_size;

    cslot cmem(gs*bpp);

    {
        uint8_t *p = (uint8_t *)cmem.data;
        ghostIO  gIO;
        gIO.load(F,g,p);
    }
}

template <typename COORD, typename FIELD>
static inline
void test_ghosts(const ghosts_of<COORD> &GG, const FIELD &F)
{
    mpi & MPI = * mpi::location();
    MPI.Printf0(stderr, "testing with ITEM_SIZE=%d\n", unsigned(sizeof(typename FIELD::type)) );
    for(size_t dim=0;dim<FIELD::DIMENSION;++dim)
    {
        const ghosts &G = GG[dim];
        if(G.prev)
        {
            MPI.Printf(stderr,"prev: %u data\n", unsigned(G.prev->size()));
            test_field_io(* G.prev, F);
        }
        else
        {
            MPI.Printf(stderr,"prev: no data\n");
        }
    }
}

YOCTO_PROGRAM_START()
{
    YOCTO_MPI_ENV();

    const layout<coord1d> full(-10,10);

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

        const field1d<char>  Fi("Fi",D,ng);
        const field1d<float> Ff("Ff",D,ng);
        const field1d< point3d<double> > Fv3("Fv3",D,ng);

        test_ghosts(G,Fi);
        test_ghosts(G,Ff);
        test_ghosts(G,Fv3);
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
    }
}
YOCTO_PROGRAM_END()

