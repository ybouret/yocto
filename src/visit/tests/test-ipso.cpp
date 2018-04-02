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
        const size_t   size;
        const size_t   rank;

        const patch1D      region1D;
        const coord1D      pbcs1D;
        const coord1D      sizes1D;
        const divide::in1D full1D;

        const patch2D      region2D;
        const coord2D      pbcs2D;
        const coord2D      sizes2D;
        const divide::in2D full2D;

        const patch3D      region3D;
        const coord3D      pbcs3D;
        const coord3D      sizes3D;
        const divide::in3D full3D;

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

        region2D(coord2D(1,1),dims.xy()),
        pbcs2D(pbcs.xy()),
        sizes2D( map_optimal_sizes(size,region2D,layers,pbcs2D) ),
        full2D(sizes2D,region2D),

        region3D(coord3D(1,1,1),dims),
        pbcs3D(pbcs),
        sizes3D(map_optimal_sizes(size,region3D,layers,pbcs3D) ),
        full3D(sizes3D,region3D)
        {


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


