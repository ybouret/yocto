#include "yocto/utest/run.hpp"
#include "yocto/fame/mesh/rectilinear.hpp"
#include "yocto/fame/mpi/ghosts.hpp"
#include "yocto/string/conv.hpp"
#include "yocto/fame/mpi/domains.hpp"
#include "yocto/fame/fields.hpp"
#include "yocto/fame/visit.hpp"
#include "yocto/code/rand32.hpp"

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
    ghosts_of<coord1d>               G1;
    mpi_ghosts<coord1d>              xch1;

    explicit Heat(VisIt &visit, const size_t Nx,const size_t Ny,const size_t Nz) :
    VisIt::Simulation(visit),
    full1(1,Nx),
    pbc1(0),
    dom1(MPI,NULL,full1,pbc1),
    mesh1("mesh1",dom1,ng),
    iof1(2),
    A1( iof1.record( new field1d<double>("A1",dom1,ng) ) ),
    B1( iof1.record( new field1d<double>("B1",dom1,ng) ) ),
    L1("L1",dom1,0),
    G1(A1),
    xch1(MPI)
    {
        {
            const box<double,coord1d> box1(0,1);
            mesh1.map_to(box1);
            xch1.prepare_for(G1,iof1);
            reset1();
        }


    }

    virtual void setMetaData(visit_handle &md)
    {
        {
            visit_handle curv1 = __visit::CurveMetaData(A1);
            VisIt_SimulationMetaData_addCurve(md,curv1);
        }

        
    }


    virtual visit_handle getVariable(const int dom, const string &id)
    {

        

        return VISIT_INVALID_HANDLE;
    }

    virtual visit_handle getCurve(const string &id)
    {

        if(id == A1.name )
        {
            return __visit::CurveData(mesh1[0],A1);
        }

        return VISIT_INVALID_HANDLE;
    }


    inline void reset1() 
    {
        const field1d<double> &X = mesh1[0];
        for(coord1d i=dom1.inner.lower;i<=dom1.inner.upper;++i)
        {
            A1[i] = X[i]       + 0.2 * _rand.sym1<double>();
            B1[i] = (1.0-X[i]) + 0.2 * _rand.sym1<double>();
        }
        xch1.perform(G1,iof1);
    }

    inline virtual void one_step()
    {
        MPI.Printf0(stderr,"heat one step\n");
        reset1();
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

    heat.loop();

}
YOCTO_UNIT_TEST_DONE()
