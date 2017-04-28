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


namespace
{
    static float rmesh_x[] = {0., 1., 2.5, 5.};
    static float rmesh_y[] = {0., 2., 2.25, 2.55,  5.};
    static int   rmesh_dims[] = {4, 5, 1};
    static int   rmesh_ndims = 2;
    static float zonal[] = {1.,2.,3.,4.,5.,6.,7.,8.,9.,10.,11.,12.};
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
        // mesh2d
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

        // zonal meta data
        {
            visit_handle m = VISIT_INVALID_HANDLE;
            if( VISIT_ERROR != VisIt_VariableMetaData_alloc(&m) )
            {

                VisIt_VariableMetaData_setName(m,"zonal");
                VisIt_VariableMetaData_setMeshName(m,"mesh2d");
                VisIt_VariableMetaData_setType(m,VISIT_VARTYPE_SCALAR);
                VisIt_VariableMetaData_setCentering(m,VISIT_VARCENTERING_ZONE);
                VisIt_SimulationMetaData_addVariable(md,m);
            }
        }

    }


    virtual visit_handle getMesh(const int domain, const string &mesh_name)
    {
        visit_handle mesh = VISIT_INVALID_HANDLE;
        if( "mesh2d" == mesh_name )
        {
             if(VisIt_RectilinearMesh_alloc(&mesh) != VISIT_ERROR)
             {
                 visit_handle hxc, hyc;
                 VisIt_VariableData_alloc(&hxc);
                 VisIt_VariableData_alloc(&hyc);
                 VisIt_VariableData_setDataF(hxc,VISIT_OWNER_SIM,1,rmesh_dims[0], rmesh_x);
                 VisIt_VariableData_setDataF(hyc,VISIT_OWNER_SIM,1,rmesh_dims[1], rmesh_y);
                 VisIt_RectilinearMesh_setCoordsXY(mesh, hxc, hyc);
             }
        }

        return mesh;
    }


    virtual visit_handle getVariable(const int domain, const string &variable_name)
    {
        visit_handle h = VISIT_INVALID_HANDLE;

        if( "zonal" == variable_name )
        {
            if(VISIT_ERROR != VisIt_VariableData_alloc(&h))
            {
                const int nTuples = (rmesh_dims[0]-1) * (rmesh_dims[1]-1);
                VisIt_VariableData_setDataF(h, VISIT_OWNER_SIM, 1, nTuples, zonal);
            }
        }

        return h;
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
