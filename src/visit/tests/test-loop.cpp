#include "yocto/utest/run.hpp"
#include "yocto/visit/interface.hpp"

using namespace yocto;

static inline
void onClicked(void *addr)
{
    std::cerr << "doNothing..." << std::endl;
}

static inline
void onValueChanged(int value, void *addr)
{
    fflush(stderr);
    std::cerr << "valueChanged=" << value << std::endl;
    std::cerr.flush();
}

static inline
void onStateChanged(int value, void *addr)
{
    fflush(stderr);
    std::cerr << "stateChanged=" << value << std::endl;
    std::cerr.flush();
}

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

    VisItUI_clicked("STEP", onClicked, NULL);
    VisItUI_valueChanged("LEVELS",onValueChanged,NULL);
    VisItUI_stateChanged("OPTION",onStateChanged,NULL);
    sim.loop();
    
}
YOCTO_UNIT_TEST_DONE()
