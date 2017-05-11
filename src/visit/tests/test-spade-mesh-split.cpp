#include "yocto/spade/visit.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace spade;

namespace
{
    class Sim : public VisIt :: Simulation
    {
    public:
        explicit Sim( const VisIt &visit ) :
        VisIt :: Simulation(visit)
        {
        }

        virtual ~Sim() throw()
        {
        }

    private:
        YOCTO_DISABLE_COPY_AND_ASSIGN(Sim);
    };
}

YOCTO_UNIT_TEST_IMPL(spade_mesh_split)
{
    const string sim_name = "spade_example";
    const string sim_comm = "testing Spade interface with visit";

    YOCTO_MPI_ENV();

    

    VisIt &visit = VisIt::Start(sim_name,
                                sim_comm,
                                MPI,
                                NULL,
                                false);



    Sim sim(visit);
    sim.loop();
}
YOCTO_UNIT_TEST_DONE()
