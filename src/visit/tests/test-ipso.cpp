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

        coord1D        fallback1D;
        const patch1D  region1D;
        const coord1D  pbcs1D;
        const coord1D  sizes1D;
        const divide::in1D full1D;

        explicit Sim(const VisIt   &visit,
                     const coord3D &dims,
                     const coord3D &pbcs,
                     const size_t   layers) :
        VisIt::Simulation(visit),
        size( MPI.CommWorldSize ),
        rank( MPI.CommWorldRank ),
        fallback1D(0),
        region1D(1,dims.x),
        pbcs1D(pbcs.x),
        sizes1D( check_sizes(mapping<coord1D>::optimal_sizes_for(size,region1D,layers,pbcs1D,&fallback1D),fallback1D,size) ),
        full1D(sizes1D,region1D)
        {


        }

        virtual ~Sim() throw()
        {
        }


    private:
        YOCTO_DISABLE_COPY_AND_ASSIGN(Sim);
        template <typename COORD>
        COORD check_sizes( const COORD sz, const COORD &fb, const coord1D np )
        {
            COORD ans = sz;
            if( __coord_prod(ans) < np )
            {
                ans = fb;
                if(__coord_prod(ans) < np )
                {
                    throw exception("too many cpus for given domain size");
                }
            }
            return ans;
        }

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


