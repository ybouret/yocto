#include "yocto/fame/visit.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace fame;

static const int ng_straight = 1;

class Sim : public VisIt::Simulation
{
public:
    const domain<coord2d> D2periodic;
    const domain<coord2d> D2straight;

    box<float,coord2d>                     B2p;
    rectilinear_mesh<float,coord2d>        mesh2p;

    box<float,coord2d>                     B2s;
    const rectilinear_mesh<double,coord2d> mesh2s;

    const domain<coord3d>                  D3periodic;
    box<double,coord3d>                    B3;
    const rectilinear_mesh<double,coord3d> mesh3p;

    const domain<coord3d>                  D3straight;
    const rectilinear_mesh<double,coord3d> mesh3s;

    typedef box<float,coord2d>::vtx   v2f;
    typedef box<double,coord3d>::vtx  v3d;

    virtual ~Sim() throw()
    {
    }

    explicit Sim( VisIt                &visit,
                 const layout<coord2d> &full2d,
                 const layout<coord3d> &full3d) :
    VisIt::Simulation(visit),
    D2periodic(MPI.CommWorldRank,MPI.CommWorldSize,NULL,full2d,coord2d(1,1)),
    D2straight(MPI.CommWorldRank,MPI.CommWorldSize,NULL,full2d,coord2d(0,0)),
    B2p( v2f(0,0),   v2f(1,1) ),
    mesh2p("mesh2periodic",D2periodic,1),
    B2s( v2f(0,0),   v2f(1,1) ),
    mesh2s("mesh2straight",D2straight,ng_straight,B2s),
    D3periodic(MPI.CommWorldRank,MPI.CommWorldSize,NULL,full3d,coord3d(1,1,1)),
    B3( v3d(0,0,0), v3d(1,1,1) ),
    mesh3p("mesh3per",D3periodic,1,B3),
    D3straight(MPI.CommWorldRank,MPI.CommWorldSize,NULL,full3d,coord3d(0,0,0)),
    mesh3s("mesh3str",D3straight,1,B3)
    {
        MPI.Printf(stderr,"mesh2p: x@[%d:%d], y@[%d:%d]\n",
                   int(mesh2p[0].outer.lower), int(mesh2p[0].outer.upper),
                   int(mesh2p[1].outer.lower), int(mesh2p[1].outer.upper));
        
        MPI.Printf(stderr,"mesh2s: x@[%d:%d], y@[%d:%d]\n",
                   int(mesh2s[0].outer.lower), int(mesh2s[0].outer.upper),
                   int(mesh2s[1].outer.lower), int(mesh2s[1].outer.upper));
        
        mesh2p.map_to(B2p);
        MPI.Printf(stderr,"mesh2s: indices_x=%d->%d, indices_y=%d->%d\n",mesh2s.imin[0],mesh2s.imax[0],mesh2s.imin[1],mesh2s.imax[1]);

        //for(size_t i=0;i<3;++i) mesh3p.imin[i] = 0 ;
        MPI.Printf(stderr,"mesh3p: indices_x=%d->%d, indices_y=%d->%d, indices_z=%d->%d\n",mesh3p.imin[0],mesh3p.imax[0],mesh3p.imin[1],mesh3p.imax[1],mesh3p.imin[2],mesh3p.imax[2]);
        MPI.Printf(stderr,"mesh3s: indices_x=%d->%d, indices_y=%d->%d, indices_z=%d->%d\n",mesh3s.imin[0],mesh3s.imax[0],mesh3s.imin[1],mesh3s.imax[1],mesh3s.imin[2],mesh3s.imax[2]);

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

        {
            visit_handle m = __visit::MeshMetaData(mesh3p);
            VisIt_MeshMetaData_setNumDomains(m,D3periodic.full.size);
            VisIt_SimulationMetaData_addMesh(md,m);
        }

        {
            visit_handle m = __visit::MeshMetaData(mesh3s);
            VisIt_MeshMetaData_setNumDomains(m,D3straight.full.size);
            VisIt_SimulationMetaData_addMesh(md,m);
        }


    }
    
    virtual visit_handle getMesh(const int     domain,
                                 const string &mesh_name)
    {
        
        if( mesh_name == "mesh2periodic" )
        {
            return __visit::MeshData(mesh2p);
        }
        
        if( mesh_name == "mesh2straight" )
        {
            return __visit::MeshData(mesh2s);
        }

        if( mesh_name == "mesh3per" )
        {
            return __visit::MeshData(mesh3p);
        }

        if( mesh_name == "mesh3str" )
        {
            return __visit::MeshData(mesh3s);
        }

        
        return VISIT_INVALID_HANDLE;
    }
    
private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Sim);
};

#include "yocto/string/conv.hpp"

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

    size_t Nx = 10;
    size_t Ny = 10;
    size_t Nz = 10;
    if(argc>1)
    {
        Nx = strconv::to_size(argv[1],"Nx");
    }
    if(argc>2)
    {
        Ny = strconv::to_size(argv[2],"Ny");
    }
    if(argc>3)
    {
        Nz = strconv::to_size(argv[3],"Nz");
    }

    const layout<coord2d> full2d( coord2d(1,1),   coord2d(Nx,Ny)    );
    const layout<coord3d> full3d( coord3d(1,1,1), coord3d(Nx,Ny,Nz) );

    Sim sim(visit,full2d,full3d);

    sim.loop();


}
YOCTO_UNIT_TEST_DONE()
