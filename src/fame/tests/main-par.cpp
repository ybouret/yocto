#include "yocto/fame/ghost.hpp"
#include "yocto/fame/layouts.hpp"
#include "yocto/program.hpp"
#include "yocto/mpi/mpi.hpp"

using namespace yocto;
using namespace fame;



YOCTO_PROGRAM_START()
{
    YOCTO_MPI_ENV();

    {
        const layout_of<coord1d> full(-10,10);
        MPI.Printf0(stderr,"IS  periodic\n");
        const domain_of<coord1d> D(MPI.CommWorldRank,MPI.CommWorldSize,NULL,full,1);
        layouts_of<coord1d>      W(D,2);
        ghosts_of<coord1d>       G(W);
        MPI.Printf(stderr,"is %2ld.%2ld: layout: inner=[%5ld:%5ld], outer=[%5ld:%5ld]\n",
                   D.full.size,D.self.rank,
                   W.inner.lower,
                   W.inner.upper,
                   W.outer.lower,
                   W.outer.upper);


#if 0
        std::cerr << "NOT periodic:" << std::endl;
        for(size_t rank=0;rank<size;++rank)
        {
            domain_of<coord1d>  D(rank,size,NULL,L,0);
            layouts_of<coord1d> W(D,1);
            display_lay(W);
            ghosts_of<coord1d> G(W);
        }
#endif

    }
}
YOCTO_PROGRAM_END()

