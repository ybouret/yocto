#include "yocto/utest/run.hpp"
#include "yocto/visit/interface.hpp"

using namespace yocto;

YOCTO_UNIT_TEST_IMPL(loop)
{

    const string sim_name = "loop";
    const string sim_comm = "testing";
    const string sim_gui  = "loop.ui";

    YOCTO_MPI_ENV();

    VisIt &visit = VisIt::Start(sim_name,
                                sim_comm,
                                MPI,
                                &sim_gui,
                                false);

    VisIt::Simulation sim(visit);

    sim.loop();
    
}
YOCTO_UNIT_TEST_DONE()
