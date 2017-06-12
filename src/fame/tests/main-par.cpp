#include "yocto/fame/ghost-io.hpp"
#include "yocto/fame/field1d.hpp"
#include "yocto/program.hpp"
#include "yocto/mpi/mpi.hpp"
#include "yocto/container/cslot.hpp"

using namespace yocto;
using namespace fame;


static size_t ng = 2;



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

        const field1d<char>              Fi("Fi",D,ng);
        const field1d<float>             Ff("Ff",D,ng);
        const field1d< point3d<double> > Fv3("Fv3",D,ng);

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

