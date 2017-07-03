#include "yocto/fame/visit.hpp"
#include "yocto/fame/mpi/domains.hpp"
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

    field2d<float>             f2p;
    field2d< point2d<double> > c2s;

    field3d<double>            f3p;

    typedef box<float,coord2d>::vtx   v2f;
    typedef box<double,coord3d>::vtx  v3d;


    const layout<coord3d>           clay;
    const whole_domain<coord3d>     cdom;
    curvilinear_mesh<float,coord3d> cmesh;

    curvilinear_mesh<double,coord2d> cmesh2p;

    const mpi_domain<coord1d>        pdom;
    point_mesh<float,coord2d>        pmesh2s;


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
    mesh3s("mesh3str",D3straight,1,B3),
    f2p("f2p",D2periodic,1),
    c2s("c2s",D2straight,ng_straight),
    f3p("f3p",D3periodic,1),
    clay( coord3d(0,0,0), coord3d(4,3,2)-coord3d(1,1,1)),
    cdom( clay, coord3d(0,0,0) ),
    cmesh("cmesh",cdom,0),
    cmesh2p("cmesh2p",D2periodic,0),
    pdom(MPI,NULL,layout<coord1d>(full2d.lower.x,full2d.upper.x),0),
    pmesh2s("pmesh2s",pdom,0)
    {
        MPI.Printf(stderr,"mesh2p: x@[%d:%d], y@[%d:%d]\n",
                   int(mesh2p[0].outer.lower), int(mesh2p[0].outer.upper),
                   int(mesh2p[1].outer.lower), int(mesh2p[1].outer.upper));

        MPI.Printf(stderr,"mesh2s: x@[%d:%d], y@[%d:%d]\n",
                   int(mesh2s[0].outer.lower), int(mesh2s[0].outer.upper),
                   int(mesh2s[1].outer.lower), int(mesh2s[1].outer.upper));

        mesh2p.map_to(B2p);
        MPI.Printf(stderr,"mesh2s: indices_x=%d->%d, indices_y=%d->%d\n",mesh2s.imin[0],mesh2s.imax[0],mesh2s.imin[1],mesh2s.imax[1]);

        MPI.Printf(stderr,"mesh3p: indices_x=%d->%d, indices_y=%d->%d, indices_z=%d->%d\n",mesh3p.imin[0],mesh3p.imax[0],mesh3p.imin[1],mesh3p.imax[1],mesh3p.imin[2],mesh3p.imax[2]);
        MPI.Printf(stderr,"mesh3s: indices_x=%d->%d, indices_y=%d->%d, indices_z=%d->%d\n",mesh3s.imin[0],mesh3s.imax[0],mesh3s.imin[1],mesh3s.imax[1],mesh3s.imin[2],mesh3s.imax[2]);


        cmesh2p.map_to(B2p);

        {
            const layout<coord2d> &outer = f2p.outer;
            for(coord1d j=outer.lower.y;j<=outer.upper.y;++j)
            {
                const float y = mesh2p[1][j];
                for(coord1d i=outer.lower.x;i<=outer.upper.x;++i)
                {
                    const float x = mesh2p[0][i];
                    f2p[j][i]   = cos(7*((x-0.5)*(y-0.5)));

                }
            }
        }

        if(true)
        {
            const layout<coord2d> &outer = c2s.outer;
            for(coord1d j=outer.lower.y;j<=outer.upper.y;++j)
            {
                const float y = mesh2s[1][j];
                for(coord1d i=outer.lower.x;i<=outer.upper.x;++i)
                {
                    const float x = mesh2s[0][i];
                    c2s[j][i].x = cos(6*x);
                    c2s[j][i].y = sin(6*y);
                }
            }
        }

        {
            const layout<coord3d> &outer = f3p.outer;
            for(coord1d k=outer.lower.z;k<=outer.upper.z;++k)
            {
                const float z = mesh3p[2][k];
                for(coord1d j=outer.lower.y;j<=outer.upper.y;++j)
                {
                    const float y = mesh3p[1][j];
                    for(coord1d i=outer.lower.x;i<=outer.upper.x;++i)
                    {
                        const float x = mesh3p[0][i];
                        f3p[k][j][i] = exp( - 0.1 * (square_of(x-0.5)+square_of(y-0.5)+square_of(z-0.5) ) );
                    }
                }
            }

        }

#if 1
        static float cmesh_x[2][3][4] = {
            {{0.,1.,2.,3.},{0.,1.,2.,3.}, {0.,1.,2.,3.}},
            {{0.,1.,2.,3.},{0.,1.,2.,3.}, {0.,1.,2.,3.}}
        };
        static  float cmesh_y[2][3][4] = {
            {{0.5,0.,0.,0.5},{1.,1.,1.,1.}, {1.5,2.,2.,1.5}},
            {{0.5,0.,0.,0.5},{1.,1.,1.,1.}, {1.5,2.,2.,1.5}}
        };
        static float cmesh_z[2][3][4] = {
            {{0.,0.,0.,0.},{0.,0.,0.,0.},{0.,0.,0.,0.}},
            {{1.,1.,1.,1.},{1.,1.,1.,1.},{1.,1.,1.,1.}}
        };
        for(int k=0;k<2;++k)
        {
            for(int j=0;j<3;++j)
            {
                for(int i=0;i<4;++i)
                {
                    cmesh[0][k][j][i] = cmesh_x[k][j][i];
                    cmesh[1][k][j][i] = cmesh_y[k][j][i];
                    cmesh[2][k][j][i] = cmesh_z[k][j][i];

                }
            }
        }
#endif

        for(coord1d i=pmesh2s[0].outer.lower;i<=pmesh2s[0].outer.upper;++i)
        {
            const float theta = float(i) * (10.0f*3.14f/180.f);
            pmesh2s[0][i] = theta * cos(theta);
            pmesh2s[1][i] = theta * sin(theta);
        }

    }

    virtual void setMetaData(visit_handle &md)
    {
        {
            visit_handle m = __visit::MeshMetaData(mesh2p);
            VisIt_SimulationMetaData_addMesh(md,m);
        }

        {
            visit_handle m = __visit::MeshMetaData(mesh2s);
            VisIt_SimulationMetaData_addMesh(md,m);
        }

        {
            visit_handle m = __visit::MeshMetaData(mesh3p);
            VisIt_SimulationMetaData_addMesh(md,m);
        }

        {
            visit_handle m = __visit::MeshMetaData(mesh3s);
            VisIt_SimulationMetaData_addMesh(md,m);
        }

        {
            if(!MPI.IsParallel)
            {
                visit_handle m = __visit::MeshMetaData(cmesh);
                VisIt_SimulationMetaData_addMesh(md,m);
            }
        }

        {
            visit_handle m = __visit::MeshMetaData(cmesh2p);
            VisIt_SimulationMetaData_addMesh(md,m);
        }


        {
            visit_handle m = __visit::MeshMetaData(pmesh2s);
            VisIt_SimulationMetaData_addMesh(md,m);
        }


        {
            visit_handle vmd = __visit::VariableMetaData(f2p,mesh2p);
            VisIt_SimulationMetaData_addVariable(md,vmd);
        }

        {
            visit_handle vmd = __visit::VariableMetaData(c2s,mesh2s);
            VisIt_SimulationMetaData_addVariable(md,vmd);
        }

        {
            visit_handle vmd = __visit::VariableMetaData(f3p,mesh3p);
            VisIt_SimulationMetaData_addVariable(md,vmd);
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

        if(!MPI.IsParallel)
        {
            if( mesh_name == "cmesh" )
            {
                return __visit::MeshData(cmesh);
            }
        }

        if( mesh_name == "cmesh2p" )
        {
            return __visit::MeshData(cmesh2p);
        }

        if( mesh_name == "pmesh2s" )
        {
            return __visit::MeshData(pmesh2s);
        }

        return VISIT_INVALID_HANDLE;
    }

    virtual visit_handle getVariable(const int domain, const string &variable_name)
    {


        if( "f2p" == variable_name )
        {
            return __visit::VariableData(f2p);
        }

        if( "c2s" == variable_name )
        {
            return __visit::VariableData(c2s);
        }

        if( "f3p" == variable_name )
        {
            return __visit::VariableData(f3p);
        }




        return VISIT_INVALID_HANDLE;
    }

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Sim);
};

#include "yocto/string/conv.hpp"

YOCTO_UNIT_TEST_IMPL(mesh)
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
