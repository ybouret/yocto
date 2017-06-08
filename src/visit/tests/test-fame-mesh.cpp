#include "yocto/fame/visit.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace fame;

class Sim : public VisIt::Simulation
{
public:
    const domain<coord2d> D2periodic;
    const domain<coord2d> D2straight;

    virtual ~Sim() throw()
    {
    }

    explicit Sim( VisIt                &visit,
                 const layout<coord2d> &full2d ) :
    VisIt::Simulation(visit),
    D2periodic(MPI.CommWorldRank,MPI.CommWorldSize,NULL,full2d,coord2d(1,1)),
    D2straight(MPI.CommWorldRank,MPI.CommWorldSize,NULL,full2d,coord2d(0,0))
    {
    }

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Sim);
};

YOCTO_UNIT_TEST_IMPL(fame_mesh)
{
    const string sim_name = "fame_mesh";
    const string sim_comm = "testing mesh";
    //const string sim_gui  = "loop.ui";

    YOCTO_MPI_ENV();

    VisIt &visit = VisIt::Start(sim_name,
                                sim_comm,
                                MPI,
                                NULL,
                                false);

    const layout<coord2d> full2d( coord2d(1,1), coord2d(16,16) );
    Sim sim(visit,full2d);

    sim.loop();


}
YOCTO_UNIT_TEST_DONE()
