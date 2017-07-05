#include "yocto/fame/visit.hpp"
#include "yocto/fame/mpi/domains.hpp"
#include "yocto/fame/mpi/ghosts.hpp"

#include "yocto/utest/run.hpp"
#include "yocto/code/rand.hpp"

using namespace yocto;
using namespace fame;
using namespace math;

static const int ng = 1;

class Sim : public VisIt::Simulation
{
public:

    //__________________________________________________________________________
    //
    //
    // domains
    //
    //__________________________________________________________________________
    const mpi_domain<coord1d> D1p;  //!< periodic
    const mpi_domain<coord1d> D1s;  //!< straight

    const mpi_domain<coord2d> D2xy; //!< xy  periodic
    const mpi_domain<coord2d> D2_x; //!< x   periodic
    const mpi_domain<coord2d> D2_y; //!< y   periodic
    const mpi_domain<coord2d> D2np; //!< not periodic

    const mpi_domain<coord3d> D3xyz; //!< xyz  periodic


    const mpi_domain<coord3d> D3_xy; //!< xy   periodic
    const mpi_domain<coord3d> D3_xz; //!< xz   periodic
    const mpi_domain<coord3d> D3_yz; //!< yz   periodic

    const mpi_domain<coord3d> D3_x;  //!< x    periodic
    const mpi_domain<coord3d> D3_y;  //!< y    periodic
    const mpi_domain<coord3d> D3_z;  //!< z    periodic

    const mpi_domain<coord3d> D3np;  //!< not  periodic

    //__________________________________________________________________________
    //
    //
    // meshes
    //
    //__________________________________________________________________________

    // support boxes
    const box<double,coord2d> box2;
    const box<double,coord3d> box3;

    //__________________________________________________________________________
    //
    // rectilinear 2d
    //__________________________________________________________________________
    rectilinear_mesh<float,coord2d> rmesh2xy;
    rectilinear_mesh<float,coord2d> rmesh2_x;
    rectilinear_mesh<float,coord2d> rmesh2_y;
    rectilinear_mesh<float,coord2d> rmesh2np;

    //__________________________________________________________________________
    //
    // curvilinear 2d
    //__________________________________________________________________________
    curvilinear_mesh<float,coord2d> cmesh2xy;
    curvilinear_mesh<float,coord2d> cmesh2np;
    const layout<coord3d>           sample_layout;
    const domain<coord3d>           sample_domain;
    curvilinear_mesh<float,coord3d> cmesh;

    //__________________________________________________________________________
    //
    // point mesh 2d
    //__________________________________________________________________________
    point_mesh<double,coord2d> pmesh2p;
    point_mesh<double,coord2d> pmesh2s;


    //__________________________________________________________________________
    //
    // 2D fields
    //__________________________________________________________________________
    fields                    f2;
    field2d<double>          &A2xy;
    field2d<point2d<float> > &B2xy;
    field2d<float>            C2np;

    //__________________________________________________________________________
    //
    //
    // mpi ops
    //
    //__________________________________________________________________________
    ghosts_of<coord1d>  G1p;
    ghosts_of<coord1d>  G1s;

    ghosts_of<coord2d>  G2xy;
    ghosts_of<coord2d>  G2np;

    mpi_ghosts<coord1d> xch1p;
    mpi_ghosts<coord1d> xch1s;

    mpi_ghosts<coord2d> xch2xy;
    mpi_ghosts<coord2d> xch2np;


    template <typename T>
    inline void reset( field2d<T> &F )
    {
        const layout<coord2d> &l = F.outer;
        for(coord1d j=l.lower.y;j<=l.upper.y;++j)
        {
            for(coord1d i=l.lower.x;i<=l.upper.x;++i)
            {
                F[j][i] = alea<T>();
            }
        }
    }

    template <typename T>
    inline void reset( field1d<T> &F )
    {
        const layout<coord1d> &l = F.outer;
        for(coord1d i=l.lower;i<=l.upper;++i)
        {
            F[i] = alea<T>();
        }
    }

    template <typename T>
    inline void reset( field3d<T> &F )
    {
        const layout<coord3d> &l = F.outer;
        for(coord1d k=l.lower.z;k<=l.upper.z;++k)
        {
            for(coord1d j=l.lower.y;j<=l.upper.y;++j)
            {
                for(coord1d i=l.lower.x;i<=l.upper.x;++i)
                {
                    F[k][j][i] = alea<T>();
                }
            }
        }
    }




    virtual ~Sim() throw()
    {
    }

    explicit Sim( VisIt                &visit,
                 const layout<coord1d> &full1d,
                 const layout<coord2d> &full2d,
                 const layout<coord3d> &full3d) :
    VisIt::Simulation(visit),
    D1p(MPI,NULL,full1d,1),
    D1s(MPI,NULL,full1d,0),

    D2xy(MPI,NULL,full2d,coord2d(1,1)),
    D2_x(MPI,NULL,full2d,coord2d(1,0)),
    D2_y(MPI,NULL,full2d,coord2d(0,1)),
    D2np(MPI,NULL,full2d,coord2d(0,0)),

    D3xyz(MPI,NULL,full3d,coord3d(1,1,1)),

    D3_xy(MPI,NULL,full3d,coord3d(1,1,0)),
    D3_xz(MPI,NULL,full3d,coord3d(1,0,1)),
    D3_yz(MPI,NULL,full3d,coord3d(0,1,1)),

    D3_x( MPI,NULL,full3d,coord3d(1,0,0)),
    D3_y( MPI,NULL,full3d,coord3d(0,1,0)),
    D3_z( MPI,NULL,full3d,coord3d(0,0,1)),

    D3np( MPI,NULL,full3d,coord3d(0,0,0)),

    box2( point2d<double>(0,0),   point2d<double>(1,1) ),
    box3( point3d<double>(0,0,0), point3d<double>(1,1,1) ),

    rmesh2xy("rmesh2xy",D2xy,ng,box2),
    rmesh2_x("rmesh2_x",D2_x,ng,box2),
    rmesh2_y("rmesh2_y",D2_y,ng,box2),
    rmesh2np("rmesh2np",D2np,ng,box2),

    cmesh2xy("cmesh2xy",D2xy,ng),
    cmesh2np("cmesh2np",D2np,ng),

    sample_layout(coord3d(0,0,0),coord3d(4,3,2)-coord3d(1,1,1)),
    sample_domain(0,1,NULL,sample_layout,coord3d(0,0,0)),
    cmesh("cmesh",sample_domain,0),

    pmesh2p("pmesh2p",D1p,ng),
    pmesh2s("pmesh2s",D1s,ng),

    f2(),
    A2xy( f2.record( new field2d<double>("A2xy",D2xy,ng) ) ),
    B2xy( f2.record( new field2d<point2d<float> >("B2xy",D2xy,ng) ) ),
    C2np( "C2np",D2np,ng),
    G1p( pmesh2p[0] ),
    G1s( pmesh2s[0] ),

    G2xy( A2xy ),
    G2np( C2np ),

    xch1p(MPI),
    xch1s(MPI),
    xch2xy(MPI),
    xch2np(MPI)
    {

        addCommand("reset", this, & Sim::onReset,true);
        addCommand("xch",this, & Sim::onExchange,true);

        xch1p.prepare_for(G1p,32);
        xch1s.prepare_for(G1s,32);

        xch2xy.prepare_for(G2xy,f2);
        xch2np.prepare_for(G2np,C2np);

        {
            const layout<coord2d> &l = A2xy.inner;
            MPI.Printf(stderr,"xy2d_inner: x:%ld->%ld, y:%ld->%ld\n", l.lower.x, l.upper.x, l.lower.y, l.upper.y);
            MPI.Newline(stderr);
        }

        {
            const layout<coord2d> &l = A2xy.outer;
            MPI.Printf(stderr,"xy2d_outer: x:%ld->%ld, y:%ld->%ld\n", l.lower.x, l.upper.x, l.lower.y, l.upper.y);
            MPI.Newline(stderr);
        }

        {
            const layout<coord2d> &l = C2np.inner;
            MPI.Printf(stderr,"np2d_inner: x:%ld->%ld, y:%ld->%ld\n", l.lower.x, l.upper.x, l.lower.y, l.upper.y);
            MPI.Newline(stderr);
        }

        {
            const layout<coord2d> &l = C2np.outer;
            MPI.Printf(stderr,"np2d_outer: x:%ld->%ld, y:%ld->%ld\n", l.lower.x, l.upper.x, l.lower.y, l.upper.y);
            MPI.Newline(stderr);
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

        reset_all();

    }

    inline void reset_all() throw()
    {
        MPI.Printf(stderr, "reset all\n");
        reset( pmesh2p[0] );
        reset( pmesh2p[1] );
        reset( pmesh2s[0] );
        reset( pmesh2s[1] );

        reset(A2xy);
        reset(B2xy);
        reset(C2np);
        //exchange_all();
    }

    inline void exchange_all()
    {
        xch1p.perform(G1p,pmesh2p[0]);
        xch1p.perform(G1p,pmesh2p[1]);

        xch1s.perform(G1s,pmesh2s[0]);
        xch1s.perform(G1s,pmesh2s[1]);

        xch2xy.perform(G2xy,f2);
        xch2np.perform(G2np,C2np);

    }

#define __mesh_decl(NAME) do { VisIt_SimulationMetaData_addMesh(md,__visit::MeshMetaData(NAME)); } while(false)
#define __field_decl(NAME,MESH) do {\
VisIt_SimulationMetaData_addVariable(md,__visit::VariableMetaData(NAME,MESH)); \
} while(false)

    virtual void setMetaData(visit_handle &md)
    {
        __mesh_decl(rmesh2xy);
        __mesh_decl(rmesh2_x);
        __mesh_decl(rmesh2_y);
        __mesh_decl(rmesh2np);
        __mesh_decl(cmesh2xy);
        __mesh_decl(cmesh2np);
        __mesh_decl(pmesh2p);
        __mesh_decl(pmesh2s);

        if(MPI.IsSerial)
        {
            __mesh_decl(cmesh);
        }

        __field_decl(A2xy,rmesh2xy);
        __field_decl(B2xy,rmesh2xy);
        __field_decl(C2np,rmesh2np);
    }

#define __mesh_impl(NAME) do { if(#NAME==meshID) { return __visit::MeshData(NAME); } } while(false)

    virtual visit_handle getMesh(const int     domain,
                                 const string &meshID)
    {

        __mesh_impl(rmesh2xy);
        __mesh_impl(rmesh2_x);
        __mesh_impl(rmesh2_y);
        __mesh_impl(rmesh2np);
        __mesh_impl(cmesh2xy);
        __mesh_impl(cmesh2np);
        __mesh_impl(pmesh2p);
        __mesh_impl(pmesh2s);

        if(MPI.IsSerial)
        {
            __mesh_impl(cmesh);
        }
        return VISIT_INVALID_HANDLE;
    }

#define __field_impl(NAME) do { if(#NAME==id) { return __visit::VariableData(NAME); } } while(false)

    virtual visit_handle getVariable(const int domain, const string &id)
    {

        __field_impl(A2xy);
        __field_impl(B2xy);
        __field_impl(C2np);
        return VISIT_INVALID_HANDLE;
    }


    void onReset(YOCTO_VISIT_SIMULATION_CALLBACK_PROTO)
    {
        //MPI.Printf(stderr,"===> %s\n", cmd.c_str() );
        reset_all();
    }

    void onExchange(YOCTO_VISIT_SIMULATION_CALLBACK_PROTO)
    {
        //MPI.Printf(stderr,"===> %s\n", cmd.c_str() );
        exchange_all();
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
    
    const layout<coord1d> full1d(1,Nx);
    const layout<coord2d> full2d( coord2d(1,1),   coord2d(Nx,Ny)    );
    const layout<coord3d> full3d( coord3d(1,1,1), coord3d(Nx,Ny,Nz) );
    
    Sim sim(visit,full1d,full2d,full3d);
    
    sim.loop();
    
    
}
YOCTO_UNIT_TEST_DONE()
