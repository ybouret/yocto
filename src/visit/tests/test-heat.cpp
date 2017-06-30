#include "yocto/utest/run.hpp"
#include "yocto/fame/mesh/rectilinear.hpp"
#include "yocto/fame/mpi/ghosts.hpp"
#include "yocto/string/conv.hpp"
#include "yocto/visit/interface.hpp"
#include "yocto/fame/mpi/domains.hpp"
#include "yocto/fame/fields.hpp"

using namespace yocto;
using namespace fame;

static const size_t ng = 1;

class Heat : public VisIt::Simulation
{
public:
    const layout<coord1d>            full1;
    const coord1d                    pbc1;
    const mpi_domain<coord1d>        dom1;
    rectilinear_mesh<double,coord1d> mesh1;
    fields                           iof1;
    field1d<double>                 &A1;
    field1d<double>                 &B1;
    field1d<double>                  L1;

    explicit Heat(VisIt &visit, const size_t Nx,const size_t Ny,const size_t Nz) :
    VisIt::Simulation(visit),
    full1(1,Nx),
    pbc1(0),
    dom1(MPI,NULL,full1,pbc1),
    mesh1("mesh1",dom1,ng),
    iof1(2),
    A1( iof1.record( new field1d<double>("A1",dom1,ng) ) ),
    B1( iof1.record( new field1d<double>("B1",dom1,ng) ) ),
    L1("L1",dom1,0)
    {

    }

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Heat);
};

YOCTO_UNIT_TEST_IMPL(heat)
{
    const string sim_name = "fame_heat";
    const string sim_comm = "testing heat";
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


    Heat heat(visit,Nx,Ny,Nz);


}
YOCTO_UNIT_TEST_DONE()
