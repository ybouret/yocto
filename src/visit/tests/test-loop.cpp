#include "yocto/utest/run.hpp"
#include "yocto/visit/interface.hpp"

using namespace yocto;

static inline
void onClicked(void *addr)
{
    std::cerr << "doNothing..." << std::endl;
}

static inline
void onValueChanged(int value, void *addr)
{
    fflush(stderr);
    std::cerr << "valueChanged=" << value << std::endl;
    std::cerr.flush();
}

static inline
void onStateChanged(int value, void *addr)
{
    fflush(stderr);
    std::cerr << "stateChanged=" << value << std::endl;
    std::cerr.flush();
}

class LoopSim : public VisIt::Simulation
{
public:
    explicit LoopSim( VisIt &visit ) :
    VisIt::Simulation(visit)
    {
    }

    virtual ~LoopSim() throw()
    {
    }

    virtual void one_step()
    {
        MPI.Printf0(stderr, "LoopSim/OneStep\n");
    }

    virtual void setMetaData(visit_handle &md)
    {
        {
            visit_handle m = VISIT_INVALID_HANDLE;
            if( VISIT_ERROR != VisIt_MeshMetaData_alloc(&m) )
            {
                VisIt_MeshMetaData_setName(m, "mesh2d");
                VisIt_MeshMetaData_setMeshType(m, VISIT_MESHTYPE_RECTILINEAR);
                VisIt_MeshMetaData_setTopologicalDimension(m, 2);
                VisIt_MeshMetaData_setSpatialDimension(m,2);

                VisIt_SimulationMetaData_addMesh(md,m);
            }
        }
    }


private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(LoopSim);
};

YOCTO_UNIT_TEST_IMPL(loop)
{

    const string sim_name = "loop";
    const string sim_comm = "testing";
    const string sim_gui  = "loop.ui";

    YOCTO_MPI_ENV();

    VisIt &visit = VisIt::Start(sim_name,
                                sim_comm,
                                MPI,
                                &sim_gui,
                                false);

    LoopSim sim(visit);

    VisItUI_clicked("STEP", onClicked, NULL);
    VisItUI_valueChanged("LEVELS",onValueChanged,NULL);
    VisItUI_stateChanged("OPTION",onStateChanged,NULL);
    sim.loop();
    
}
YOCTO_UNIT_TEST_DONE()
