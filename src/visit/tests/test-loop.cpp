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
    //static int   rmesh_ndims = 2;
    static float zonal[] = {1.,2.,3.,4.,5.,6.,7.,8.,9.,10.,11.,12.};

    // Curvilinear mesh
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
    static int cmesh_dims[] = {4, 3, 2};
    //static int cmesh_ndims = 3;
    static double nodal[2][3][4] = {
        {{1.,2.,3.,4.},{5.,6.,7.,8.},{9.,10.,11.,12}},
        {{13.,14.,15.,16.},{17.,18.,19.,20.},{21.,22.,23.,24.}}
    };
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
                VisIt_MeshMetaData_setXUnits(m, "cm");
                VisIt_MeshMetaData_setYUnits(m, "cm");
                VisIt_MeshMetaData_setXLabel(m, "Width");
                VisIt_MeshMetaData_setYLabel(m, "Height");
                VisIt_SimulationMetaData_addMesh(md,m);
            }
        }

        // mesh3d
        {
            visit_handle m2 = VISIT_INVALID_HANDLE;
            if( VISIT_ERROR != VisIt_MeshMetaData_alloc(&m2) )
            {
                VisIt_MeshMetaData_setName(m2, "mesh3d");
                VisIt_MeshMetaData_setMeshType(m2, VISIT_MESHTYPE_CURVILINEAR);
                VisIt_MeshMetaData_setTopologicalDimension(m2, 3);
                VisIt_MeshMetaData_setSpatialDimension(m2, 3);
                VisIt_MeshMetaData_setXUnits(m2, "cm");
                VisIt_MeshMetaData_setYUnits(m2, "cm");
                VisIt_MeshMetaData_setZUnits(m2, "cm");
                VisIt_MeshMetaData_setXLabel(m2, "Width");
                VisIt_MeshMetaData_setYLabel(m2, "Height");
                VisIt_MeshMetaData_setZLabel(m2, "Depth");
                VisIt_SimulationMetaData_addMesh(md, m2);
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

        // nodal meta data
        {
            visit_handle vmd = VISIT_INVALID_HANDLE;
            if( VISIT_ERROR != VisIt_VariableMetaData_alloc(&vmd) )
            {
                VisIt_VariableMetaData_setName(vmd, "nodal");
                VisIt_VariableMetaData_setMeshName(vmd, "mesh3d");
                VisIt_VariableMetaData_setType(vmd, VISIT_VARTYPE_SCALAR);
                VisIt_VariableMetaData_setCentering(vmd, VISIT_VARCENTERING_NODE);
                VisIt_SimulationMetaData_addVariable(md, vmd);
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

        if( "mesh3d" == mesh_name )
        {
            if(VisIt_CurvilinearMesh_alloc(&mesh) != VISIT_ERROR)
            {
                visit_handle hxc, hyc, hzc;
                const int nn = cmesh_dims[0] * cmesh_dims[1] * cmesh_dims[2];
                VisIt_VariableData_alloc(&hxc);
                VisIt_VariableData_alloc(&hyc);
                VisIt_VariableData_alloc(&hzc);
                VisIt_VariableData_setDataF(hxc, VISIT_OWNER_SIM, 1, nn, (float*)cmesh_x);
                VisIt_VariableData_setDataF(hyc, VISIT_OWNER_SIM, 1, nn, (float*)cmesh_y);
                VisIt_VariableData_setDataF(hzc, VISIT_OWNER_SIM, 1, nn, (float*)cmesh_z);
                VisIt_CurvilinearMesh_setCoordsXYZ(mesh, cmesh_dims, hxc, hyc, hzc);
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

        if( "nodal" == variable_name )
        {
            if(VISIT_ERROR != VisIt_VariableData_alloc(&h))
            {
                const int nTuples = cmesh_dims[0] * cmesh_dims[1] * cmesh_dims[2];
                const int nComponents = 1;
                VisIt_VariableData_setDataD(h, VISIT_OWNER_SIM, nComponents,
                                            nTuples, (double*)nodal);
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
