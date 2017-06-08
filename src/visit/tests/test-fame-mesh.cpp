#include "yocto/fame/visit.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace fame;

class Sim : public VisIt::Simulation
{
public:
    const domain<coord2d> D2periodic;
    const domain<coord2d> D2straight;
    rectilinear_mesh<float,coord2d>  mesh2p;
    rectilinear_mesh<double,coord2d> mesh2s;
    box<float,coord2d>                B2f;
    box<double,coord2d>               B2d;

    typedef box<float,coord2d>::vtx   v2f;
    typedef box<double,coord2d>::vtx  v2d;

    virtual ~Sim() throw()
    {
    }

    explicit Sim( VisIt                &visit,
                 const layout<coord2d> &full2d ) :
    VisIt::Simulation(visit),
    D2periodic(MPI.CommWorldRank,MPI.CommWorldSize,NULL,full2d,coord2d(1,1)),
    D2straight(MPI.CommWorldRank,MPI.CommWorldSize,NULL,full2d,coord2d(0,0)),
    mesh2p("mesh2p",D2periodic,1),
    mesh2s("mesh2s",D2straight,1),
    B2f( v2f(-1,-1), v2f(1,1) ),
    B2d( v2d(0,0), v2d(1,1) )
    {
        MPI.Printf(stderr,"mesh2p: x@[%d:%d], y@[%d:%d]\n",
                   int(mesh2p[0].outer.lower), int(mesh2p[0].outer.upper),
                   int(mesh2p[1].outer.lower), int(mesh2p[1].outer.upper));
        MPI.Printf(stderr,"mesh2s: x@[%d:%d], y@[%d:%d]\n",
                   int(mesh2s[0].outer.lower), int(mesh2s[0].outer.upper),
                   int(mesh2s[1].outer.lower), int(mesh2s[1].outer.upper));
        
        mesh2p.map_to(B2f);
        mesh2s.map_to(B2d);
    }

    virtual void setMetaData(visit_handle &md)
    {
        {
            visit_handle m = __visit::MeshMetaData(mesh2p);
            VisIt_MeshMetaData_setNumDomains(m,D2periodic.full.size);
            VisIt_SimulationMetaData_addMesh(md,m);
        }
        
        {
            visit_handle m = __visit::MeshMetaData(mesh2s);
            VisIt_MeshMetaData_setNumDomains(m,D2straight.full.size);
            VisIt_SimulationMetaData_addMesh(md,m);
        }
    }
    
    virtual visit_handle getMesh(const int     domain,
                                 const string &mesh_name)
    {
        
        if( mesh_name == "mesh2p" )
        {
            return __visit::MeshData(mesh2p);
        }
        
        if( mesh_name == "mesh2s" )
        {
            return __visit::MeshData(mesh2s);
        }
        
        return VISIT_INVALID_HANDLE;
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

    const layout<coord2d> full2d( coord2d(1,1), coord2d(20,15) );
    Sim sim(visit,full2d);

    sim.loop();


}
YOCTO_UNIT_TEST_DONE()