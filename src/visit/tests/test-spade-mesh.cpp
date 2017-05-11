#include "yocto/spade/visit.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace spade;

namespace
{
    typedef RectilinearMesh<float,coord2D> Mesh2D;
    typedef Mesh2D::box_type               Box2D;
    typedef Box2D::vtx                     V2D;

    typedef RectilinearMesh<double,coord3D> Mesh3D;
    typedef Mesh3D::box_type                Box3D;
    typedef Box3D::vtx                      V3D;


    class Sim : public VisIt::Simulation
    {
    public:
        const Layout2D L2;
        const Ghosts2D G2;
        const Box2D    B2;
        Mesh2D         mesh2;

        const Layout3D L3;
        const Ghosts3D G3;
        const Box3D    B3;
        Mesh3D         mesh3;

        explicit Sim(const VisIt &visit) :
        VisIt::Simulation(visit),
        L2( coord2D(1,1), coord2D(10,15) ),
        G2( MPI.CommWorldRank ),
        B2( V2D(-1,-1), V2D(1,1) ),
        mesh2( "mesh2", L2,G2,&B2),
        L3( coord3D(1,1,1), coord3D(30,20,10) ),
        G3( MPI.CommWorldRank ),
        B3( V3D(-3,-2,-1), V3D(3,2,1) ),
        mesh3( "mesh3", L3, G3, &B3 )
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

            {
                visit_handle m3 = __visit::MeshMetaData(mesh3);
                // TODO: add other meta data
                VisIt_SimulationMetaData_addMesh(md,m3);
            }
        }

        virtual visit_handle getMesh(const int     domain,
                                     const string &mesh_name)
        {

            if( mesh_name == "mesh2" )
            {
                return __visit::MeshData(mesh2);
            }

            if( mesh_name == "mesh3" )
            {
                return __visit::MeshData(mesh3);
            }

            return VISIT_INVALID_HANDLE;
        }



    private:
        YOCTO_DISABLE_COPY_AND_ASSIGN(Sim);
    };
}

YOCTO_UNIT_TEST_IMPL(spade_mesh)
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
