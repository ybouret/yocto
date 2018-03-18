#include "yocto/utest/run.hpp"
#include "yocto/string/conv.hpp"


YOCTO_UNIT_TEST_IMPL(heat)
{
}
YOCTO_UNIT_TEST_DONE()

#if 0
using namespace yocto;
using namespace fame;

//static const size_t ng = 1;

class Heat : public VisIt::Simulation
{
public:
    const layout<coord1d>             full1;
    const mpi_domain<coord1d>         dom1;

    inline virtual ~Heat() throw()
    {
    }


    inline Heat( const VisIt &visit, const size_t Nx ) :
    VisIt::Simulation(visit),
    full1(1,Nx),
    dom1(MPI,NULL,full1,0)
    {

    }

    virtual void setMetaData(visit_handle &md)
    {

    }


    virtual visit_handle getVariable(const int dom, const string &id)
    {

        

        return VISIT_INVALID_HANDLE;
    }

    virtual visit_handle getMesh(const int     domain,
                                 const string &meshID)
    {

        return VISIT_INVALID_HANDLE;
    }


    inline void reset1()
    {

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

    Heat heat(visit,Nx);


}
YOCTO_UNIT_TEST_DONE()
#endif

