#include "yocto/ipso/visit.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/string/conv.hpp"
#include "yocto/ipso/mpi.hpp"

using namespace yocto;
using namespace ipso;

namespace
{

    class Sim : public VisIt::Simulation
    {
    public:
        typedef point2d<float> v2d;
        typedef point3d<float> v3d;

        const size_t   size;
        const size_t   rank;

        const patch1D             region1D;
        const coord1D             pbcs1D;
        const coord1D             sizes1D;
        const divide::in1D        full1D;
        const axis_names          names1D;
        mpi_workspace<coord1D>    W1D;
        point_mesh<float,1>       pmesh1;
        rectilinear_mesh<float,1> rmesh1;
        curvilinear_mesh<float,1> cmesh1;

        const patch2D             region2D;
        const coord2D             pbcs2D;
        const coord2D             sizes2D;
        const divide::in2D        full2D;
        const axis_names          names2D;
        mpi_workspace<coord2D>    W2D;
        point_mesh<float,2>       pmesh2;
        rectilinear_mesh<float,2> rmesh2;
        curvilinear_mesh<float,2> cmesh2;

        const patch3D             region3D;
        const coord3D             pbcs3D;
        const coord3D             sizes3D;
        const divide::in3D        full3D;
        const axis_names          names3D;
        mpi_workspace<coord3D>    W3D;
        point_mesh<float,3>       pmesh3;
        rectilinear_mesh<float,3> rmesh3;
        curvilinear_mesh<float,3> cmesh3;

        explicit Sim(const VisIt   &visit,
                     const coord3D &dims,
                     const coord3D &pbcs,
                     const size_t   layers) :
        VisIt::Simulation(visit),
        size( MPI.CommWorldSize ),
        rank( MPI.CommWorldRank ),

        region1D(1,dims.x),
        pbcs1D(pbcs.x),
        sizes1D( map_optimal_sizes(size,region1D,layers,pbcs1D) ),
        full1D(sizes1D,region1D),
        names1D("x"),
        W1D(MPI,full1D,layers,pbcs1D),
        pmesh1("pmesh1",names1D,W1D),
        rmesh1("rmesh1",names1D,W1D),
        cmesh1("cmesh1",names1D,W1D),

        region2D(coord2D(1,1),dims.xy()),
        pbcs2D(pbcs.xy()),
        sizes2D( map_optimal_sizes(size,region2D,layers,pbcs2D) ),
        full2D(sizes2D,region2D),
        names2D("x,y"),
        W2D(MPI,full2D,layers,pbcs2D),
        pmesh2("pmesh2",names2D,W1D),
        rmesh2("rmesh2",names2D,W2D),
        cmesh2("cmesh2",names2D,W2D),

        region3D(coord3D(1,1,1),dims),
        pbcs3D(pbcs),
        sizes3D(map_optimal_sizes(size,region3D,layers,pbcs3D) ),
        full3D(sizes3D,region3D),
        names3D("x,y,z"),
        W3D(MPI,full3D,layers,pbcs3D),
        pmesh3("pmesh3",names3D,W1D),
        rmesh3("rmesh3",names3D,W3D),
        cmesh3("cmesh3",names3D,W3D)

        {
            MPI.Printf(stderr, "sub1D: %d->%d\n", int(W1D.outer.lower), int(W1D.outer.upper) );
            MPI.Printf(stderr, "sub2D: %d->%d | %d->%d\n",
                       int(W2D.outer.lower.x), int(W2D.outer.upper.x),
                       int(W2D.outer.lower.y), int(W2D.outer.upper.y));

            {
                box<float,1> b1D(0,1);
                pmesh1.map_regular(0,1,full1D);
                rmesh1.map_regular(b1D,full1D);
                cmesh1.map_regular(b1D,full1D);
            }

            {
                box<float,2> b2d( v2d(0,0), v2d(1,1) );
                rmesh2.map_regular(b2d,full2D);
                pmesh2.map_circle(full1D);
                cmesh2.map_regular(b2d,full2D);
            }

            {
                box<float,3> b3d( v3d(0,0,0), v3d(1,1,1) );
                rmesh3.map_regular(b3d,full3D);
                pmesh3.map_circle(full1D);
                cmesh3.map_regular(b3d,full3D);
            }

        }

        // adding meshes
        virtual void setMetaData(visit_handle &md)
        {
            (void)__visit::add_mesh_metadata(md,pmesh1);
            (void)__visit::add_mesh_metadata(md,rmesh1);
            (void)__visit::add_mesh_metadata(md,cmesh1);

            (void)__visit::add_mesh_metadata(md,pmesh2);
            (void)__visit::add_mesh_metadata(md,rmesh2);
            (void)__visit::add_mesh_metadata(md,cmesh2);

            (void)__visit::add_mesh_metadata(md,pmesh3);
            (void)__visit::add_mesh_metadata(md,rmesh3);
            (void)__visit::add_mesh_metadata(md,cmesh3);

        }

        // getting meshes
        virtual visit_handle getMesh(const int , const string &mesh_name)
        {

            if(mesh_name == "pmesh2")
            {
                return __visit::get_mesh(pmesh2);
            }

            if(mesh_name == "pmesh3")
            {
                return __visit::get_mesh(pmesh3);
            }

            if(mesh_name == "rmesh2")
            {
                return __visit::get_mesh(rmesh2);
            }

            if(mesh_name == "rmesh3")
            {
                return __visit::get_mesh(rmesh3);
            }

            if(mesh_name == "cmesh2")
            {
                return __visit::get_mesh(cmesh2);
            }

            if(mesh_name == "cmesh3")
            {
                return __visit::get_mesh(cmesh3);
            }


            return VISIT_INVALID_HANDLE;
        }

        virtual ~Sim() throw()
        {
        }


    private:
        YOCTO_DISABLE_COPY_AND_ASSIGN(Sim);
    };

}

YOCTO_UNIT_TEST_IMPL(ipso)
{
    YOCTO_MPI_ENV();
    const string sim_name = "ipso";
    const string sim_comm = "testing ipso API";
    //const string sim_gui  = "loop.ui";

    // prepare VisIt
    VisIt &visit = VisIt::Start(sim_name,
                                sim_comm,
                                MPI,
                                NULL,
                                false);


    // prepare args
    if(argc<=3)
    {
        throw exception("usage: %s DIMS PBCS LAYERS", argv[0]);
    }
    const coord3D dims   = __coord_parser::get<coord3D>(argv[1],"dims");
    const coord3D pbcs   = __coord_parser::get<coord3D>(argv[2],"pbcs");
    const size_t  layers = strconv::to_size(argv[3],"layers");

    Sim sim(visit,dims,pbcs,layers);

    sim.loop();

}
YOCTO_UNIT_TEST_DONE()


