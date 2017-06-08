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
    const string sim_name = "spade_split_example";
    const string sim_comm = "testing Spade interface with visit";

    YOCTO_MPI_ENV();

    

    VisIt &visit = VisIt::Start(sim_name,
                                sim_comm,
                                MPI,
                                NULL,
                                false);


    
    // 2D
    Layout2D       full( coord2D(1,1), coord2D(100,100) );
    const coord2D  cpus = Split::DispatchCPUs(full,MPI.CommWorldSize);
    Split::In2D    in2D(full,cpus);
    const Layout2D sim_layout = in2D(MPI.CommWorldRank);
    MPI.Printf(stderr, "width=%ld %ld\n", sim_layout.width.x, sim_layout.width.y);
    
    Sim sim(visit);
    sim.loop();
}
YOCTO_UNIT_TEST_DONE()
