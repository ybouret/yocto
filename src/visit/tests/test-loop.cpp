#include "yocto/utest/run.hpp"
#include "yocto/visit/sim.hpp"

using namespace yocto;

YOCTO_UNIT_TEST_IMPL(loop)
{

    const string sim_name = "loop";
    const string sim_comm = "";
    YOCTO_MPI_ENV();

    VisIt &sim = VisIt::Start(sim_name,
                              sim_comm,
                              MPI);

}
YOCTO_UNIT_TEST_DONE()
