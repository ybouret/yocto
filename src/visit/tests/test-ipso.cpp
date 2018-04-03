#include "yocto/ipso/visit.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/string/conv.hpp"

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

        const patch1D         region1D;
        const coord1D         pbcs1D;
        const coord1D         sizes1D;
        const divide::in1D    full1D;
        const axis_names      names1D;
        const subset<coord1D> sub1D;
        point_mesh<float,1>   pmesh1;
        rectilinear_mesh<float,1> rmesh1;

        const patch2D      region2D;
        const coord2D      pbcs2D;
        const coord2D      sizes2D;
        const divide::in2D full2D;
        const axis_names   names2D;
        const subset<coord2D> sub2D;
        point_mesh<float,2>       pmesh2;
        rectilinear_mesh<float,2> rmesh2;

        const patch3D         region3D;
        const coord3D         pbcs3D;
        const coord3D         sizes3D;
        const divide::in3D    full3D;
        const axis_names      names3D;
        const subset<coord3D> sub3D;
        point_mesh<float,3>       pmesh3;
        rectilinear_mesh<float,3> rmesh3;

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
        sub1D(full1D,rank,layers,pbcs1D,true),
        pmesh1("pmesh1",names1D,sub1D),
        rmesh1("rmesh1",names1D,sub1D),

        region2D(coord2D(1,1),dims.xy()),
        pbcs2D(pbcs.xy()),
        sizes2D( map_optimal_sizes(size,region2D,layers,pbcs2D) ),
        full2D(sizes2D,region2D),
        names2D("x,y"),
        sub2D(full2D,rank,layers,pbcs2D,true),
        pmesh2("pmesh2",names2D,sub1D),
        rmesh2("rmesh2",names2D,sub2D),

        region3D(coord3D(1,1,1),dims),
        pbcs3D(pbcs),
        sizes3D(map_optimal_sizes(size,region3D,layers,pbcs3D) ),
        full3D(sizes3D,region3D),
        names3D("x,y,z"),
        sub3D(full3D,rank,layers,pbcs3D,true),
        pmesh3("pmesh3",names3D,sub1D),
        rmesh3("rmesh3",names3D,sub3D)

        {
            {
                box<float,1> b1D(0,1);
                pmesh1.map_regular(0,1,sub1D.inner);
                rmesh1.map_regular(b1D,sub1D.inner);
            }

            {
                box<float,2> b2d( v2d(0,0), v2d(1,1) );
                rmesh2.map_regular(b2d,sub2D.inner);
                pmesh2.map_circle(sub1D.outer);
            }

            {
                box<float,3> b3d( v3d(0,0,0), v3d(1,1,1) );
                rmesh3.map_regular(b3d,sub3D.inner);
                pmesh3.map_circle(sub1D.outer);
            }

        }

        virtual void setMetaData(visit_handle &md)
        {
            // adding meshes
            __visit::add_mesh_metadata(md,pmesh1);
            __visit::add_mesh_metadata(md,rmesh1);

            __visit::add_mesh_metadata(md,pmesh2);
            __visit::add_mesh_metadata(md,rmesh2);

            __visit::add_mesh_metadata(md,pmesh3);
            __visit::add_mesh_metadata(md,rmesh3);

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


