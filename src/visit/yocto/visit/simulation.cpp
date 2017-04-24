#include "yocto/visit/interface.hpp"

namespace yocto
{
    VisIt::Simulation:: ~Simulation() throw()
    {
    }

    VisIt:: Simulation:: Simulation( const VisIt &visit ) :
    MPI(visit.MPI),
    runMode(VISIT_SIMMODE_STOPPED),
    cycle(0),
    runTime(0),
    done(false)
    {
    }
}
