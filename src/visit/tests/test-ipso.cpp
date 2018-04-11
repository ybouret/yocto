#include "yocto/ipso/visit.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/string/conv.hpp"
#include "yocto/ipso/mpi.hpp"
#include "yocto/code/alea.hpp"

using namespace yocto;
using namespace ipso;

namespace
{

    typedef visit_field1D<double> F1D;
    typedef visit_field2D<float>  F2D;
    typedef visit_field3D<float>  F3D;
    typedef point2d<float>  v2d;
    typedef point3d<float>  v3d;
    typedef visit_field2D< v2d >  F2DV;
    typedef visit_field3D< v3d >  F3DV;

    class Sim : public VisIt::Simulation
    {
    public:


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
        F1D                      &f1d;
        fields                    var1D;

        const patch2D             region2D;
        const coord2D             pbcs2D;
        const coord2D             sizes2D;
        const divide::in2D        full2D;
        const axis_names          names2D;
        mpi_workspace<coord2D>    W2D;
        point_mesh<float,2>       pmesh2;
        rectilinear_mesh<float,2> rmesh2;
        curvilinear_mesh<float,2> cmesh2;
        F2D                      &f2d;
        F2DV                     &f2dv;
        fields                    var2D;

        const patch3D             region3D;
        const coord3D             pbcs3D;
        const coord3D             sizes3D;
        const divide::in3D        full3D;
        const axis_names          names3D;
        mpi_workspace<coord3D>    W3D;
        point_mesh<float,3>       pmesh3;
        rectilinear_mesh<float,3> rmesh3;
        curvilinear_mesh<float,3> cmesh3;
        F3D  &                    f3d;
        F3DV &                    f3dv;
        fields                    var3D;

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
        f1d( W1D.create<F1D>( "f1d" ) ),

        region2D(coord2D(1,1),dims.xy()),
        pbcs2D(pbcs.xy()),
        sizes2D( map_optimal_sizes(size,region2D,layers,pbcs2D) ),
        full2D(sizes2D,region2D),
        names2D("x,y"),
        W2D(MPI,full2D,layers,pbcs2D),
        pmesh2("pmesh2",names2D,W1D),
        rmesh2("rmesh2",names2D,W2D),
        cmesh2("cmesh2",names2D,W2D),
        f2d( W2D.create<F2D>( "f2d" ) ),
        f2dv( W2D.create<F2DV>( "f2dv" ) ),

        region3D(coord3D(1,1,1),dims),
        pbcs3D(pbcs),
        sizes3D(map_optimal_sizes(size,region3D,layers,pbcs3D) ),
        full3D(sizes3D,region3D),
        names3D("x,y,z"),
        W3D(MPI,full3D,layers,pbcs3D),
        pmesh3("pmesh3",names3D,W1D),
        rmesh3("rmesh3",names3D,W3D),
        cmesh3("cmesh3",names3D,W3D),
        f3d(  W3D.create<F3D>( "f3d" )   ),
        f3dv( W3D.create<F3DV>( "f3dv" ) )

        {
            MPI.Printf(stderr, "sub1D: %d->%d\n", int(W1D.outer.lower), int(W1D.outer.upper) );


            MPI.Printf(stderr, "sub2D: %d->%d | %d->%d\n",
                       int(W2D.outer.lower.x), int(W2D.outer.upper.x),
                       int(W2D.outer.lower.y), int(W2D.outer.upper.y));

            MPI.Printf(stderr, "rmesh2d real_indices: %d-%d, %d-%d\n",
                       rmesh2.realIndices.imin[1],rmesh2.realIndices.imax[1],
                       rmesh2.realIndices.imin[2],rmesh2.realIndices.imax[2]);

            MPI.Printf(stderr, "rmesh3d real_indices: %d-%d, %d-%d, %d-%d\n",
                       rmesh3.realIndices.imin[1],rmesh3.realIndices.imax[1],
                       rmesh3.realIndices.imin[2],rmesh3.realIndices.imax[2],
                       rmesh3.realIndices.imin[3],rmesh3.realIndices.imax[3]);

            {
                box<float,1> b1D(0,1);
                pmesh1.map_regular(0,1,full1D);
                rmesh1.map_regular(b1D,full1D);
                cmesh1.map_regular(b1D,full1D);
                f1d.ld( rank+1 );
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

            var1D.append(f1d);
            W1D.allocate_swaps_for(f1d);

            var2D.append(f2d);
            var2D.append(f2dv);
            W2D.allocate_swaps_for(var2D);

            var3D.append(f3d);
            var3D.append(f3dv);
            W3D.allocate_swaps_for(var3D);

            addCommand("init", this, & Sim::init, true);
            addCommand("sync", this, & Sim::sync, true);

            init_inner();
        }

        void init_inner()
        {
            {
                f1d.ld( rank+1 );
            }

            float phi_x = 6.3f * alea.get<float>();
            float phi_y = 6.3f * alea.get<float>();
            float scale = 1.0f + alea.get<float>();
            MPI.Bcast(phi_x,0, MPI_COMM_WORLD);
            MPI.Bcast(phi_y,0, MPI_COMM_WORLD);
            MPI.Bcast(scale,0, MPI_COMM_WORLD);

            {


                f2d.ld( rank+1 );

                f2dv.ldz();
                const field1D<float> & _X = rmesh2.X();
                const field1D<float> & _Y = rmesh2.Y();
                for(coord1D j=W2D.inner.lower.y;j<=W2D.inner.upper.y;++j)
                {
                    const float y = _Y[j]-0.5f;
                    for(coord1D i=W2D.inner.lower.x;i<=W2D.inner.upper.x;++i)
                    {
                        v2d &v = f2dv[j][i];
                        const float x = _X[i]-0.5f;
                        const float fac = exp( -scale*(x*x+y*y) );
                        v.x = cosf(6.0f*x+phi_x)*fac;
                        v.y = sinf(6.0f*y+phi_y)*fac;
                    }
                }
            }

            {
                f3d.ld(rank+1);
                f3dv.ldz();
                const field1D<float> & _X = rmesh3.X();
                const field1D<float> & _Y = rmesh3.Y();
                const field1D<float> & _Z = rmesh3.Z();
                
                for(coord1D k=W3D.inner.lower.z;k<=W3D.inner.upper.z;++k)
                {
                    const float z = _Z[k] - 0.5f;
                    for(coord1D j=W3D.inner.lower.y;j<=W3D.inner.upper.y;++j)
                    {
                        const float y = _Y[j]-0.5f;
                        for(coord1D i=W3D.inner.lower.x;i<=W3D.inner.upper.x;++i)
                        {
                            v3d &v = f3dv[k][j][i];
                            const float x = _X[i]-0.5f;
                            const float fac = exp( -scale*(x*x+y*y+z*z) );
                            v.x = cosf(6.0f*x+phi_x)*fac;
                            v.y = sinf(6.0f*y+phi_y)*fac;
                            v.z = fac;
                        }
                    }
                }
            }
        }


        // adding meshes
        virtual void setMetaData(visit_handle &md)
        {
            (void)__visit::add_mesh(md,pmesh1);
            (void)__visit::add_mesh(md,rmesh1);
            (void)__visit::add_mesh(md,cmesh1);

            (void)__visit::add_mesh(md,pmesh2);
            {
                visit_handle m = __visit::add_mesh(md,rmesh2);
                VisIt_MeshMetaData_setXLabel(m, "x2d");
            }
            (void)__visit::add_mesh(md,cmesh2);

            (void)__visit::add_mesh(md,pmesh3);
            (void)__visit::add_mesh(md,rmesh3);
            (void)__visit::add_mesh(md,cmesh3);

            (void) f2d. add(md,rmesh2);
            (void) f2dv.add(md,cmesh2);
            (void) f3d. add(md,cmesh3);
            (void) f3dv.add(md,rmesh3);

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

        // getting data
        virtual visit_handle getVariable(int,const string &id)
        {
            if(id=="f2d")  return f2d. get();
            if(id=="f2dv") return f2dv.get();

            if(id=="f3d")  return f3d. get();
            if(id=="f3dv") return f3dv.get();

            return VISIT_INVALID_HANDLE;
        }

        virtual ~Sim() throw()
        {
        }

        void init(YOCTO_VISIT_SIMULATION_CALLBACK_PROTO)
        {
            init_inner();
            update();
        }

        void sync(YOCTO_VISIT_SIMULATION_CALLBACK_PROTO)
        {
            W1D.synchronize(var1D);
            W2D.synchronize(var2D);
            W3D.synchronize(var3D);
            update();
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


