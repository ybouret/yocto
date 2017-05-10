#include "yocto/spade/visit.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace spade;

namespace
{
    typedef RectilinearMesh<float,coord2D> Mesh2D;
    typedef Mesh2D::box_type               Box2D;
    typedef Box2D::vtx                     V2D;

    class Sim : public VisIt::Simulation
    {
    public:
        const Layout2D L2;
        const Ghosts2D G2;
        const Box2D    B2;
        Mesh2D         mesh2;

        explicit Sim(const VisIt &visit) :
        VisIt::Simulation(visit),
        L2( coord2D(1,1), coord2D(10,15) ),
        G2(0),
        B2( V2D(-1,-1), V2D(1,1) ),
        mesh2( "mesh2", L2,G2,&B2)
        {
        }

        virtual ~Sim() throw()
        {
        }

        virtual void one_step()
        {
            MPI.Printf0(stderr, "Sim/OneStep\n");
        }

        virtual void setMetaData(visit_handle &md)
        {
            {
                visit_handle m2 = __visit::MeshMetaData(mesh2);
                // TODO: add other meta data
                VisIt_SimulationMetaData_addMesh(md,m2);
            }
        }

        virtual visit_handle getMesh(const int     domain,
                                     const string &mesh_name)
        {

            if( mesh_name == "mesh2" )
            {
                return __visit::MeshData(mesh2);
            }

            return VISIT_INVALID_HANDLE;
        }



    private:
        YOCTO_DISABLE_COPY_AND_ASSIGN(Sim);
    };
}

YOCTO_UNIT_TEST_IMPL(spade)
{
    const string sim_name = "SPADE_example";
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
