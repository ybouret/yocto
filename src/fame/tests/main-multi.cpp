#include "yocto/fame/mpi.hpp"
#include "yocto/program.hpp"

using namespace yocto;
using namespace fame;

YOCTO_PROGRAM_START()
{
    YOCTO_MPI_ENV();
    const coord1d ng = 2;
    
    {
        const layout<coord1d> full(1,32);
        const domain<coord1d> D(MPI.CommWorldRank,MPI.CommWorldSize,NULL,full,1);
        
        field1d<double>::pointer F1( new field1d<double>("F1",D,ng) );
        ghosts_of<coord1d>       G(*F1);
        field1d<int>::pointer    F2( new field1d<int>("F2",D,ng));
    }
    
}
YOCTO_PROGRAM_END()
