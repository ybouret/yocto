
#include "yocto/ipso/visit.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/string/conv.hpp"
#include "yocto/ipso/mpi.hpp"
#include "yocto/code/alea.hpp"

using namespace yocto;
using namespace ipso;
using namespace math;


namespace {

    typedef field2D<double>            Field;
    typedef patch2D                    Patch;
    typedef mpi_workspace<coord2D>     Workspace;
    typedef rectilinear_mesh<double,2> Mesh;
    typedef box<double,2>              Box;
    typedef Box::vtx                   Vertex;


    // VisIt agnostic Code
    class Heat1D
    {
    public:
        static const size_t NumGhosts=1;

        const size_t       size;
        const size_t       rank;
        const Patch        region;
        const coord2D      pbcs;
        const coord2D      sizes;
        const divide::in2D full;
        const axis_names   names;
        Workspace          fields;
        Mesh               rmesh;
        const Patch        inner;
        Field             &A;
        Field             &L;
        double             tmx;

        explicit Heat1D(const mpi   &MPI,
                        const size_t NX) :
        size( MPI.CommWorldSize ),
        rank( MPI.CommWorldRank ),
        region( coord2D(0,0), coord2D(NX,1) ),
        pbcs(0,0),
        sizes( map_optimal_sizes(size,region,NumGhosts,pbcs) ),
        full( sizes, region ),
        names("x,T"),
        fields(MPI,full,NumGhosts,pbcs),
        rmesh("mesh",names,fields),
        inner( fields.inner ),
        A( fields.create<Field>("A") ),
        L( fields.create<Field>("L") ),
        tmx(0)
        {
            const Box b( Vertex(0,0), Vertex(1,0.2) );
            rmesh.map_regular(b,full);

            alea.initialize();
            fields.allocate_swaps_for(A);

            initialize();
        }

        virtual ~Heat1D() throw()
        {
        }

        inline double A0(double t)
        {
            return Square( Sin(t) );
        }

        void initialize()
        {
            double xc = alea.to<double>();
            fields.MPI.Bcast(xc,0,MPI_COMM_WORLD);
            const field1D<double> &X = rmesh.X();
            for(coord1D i=inner.lower.x;i<=inner.upper.x;++i)
            {
                const double dx = X[i] - xc;
                A[0][i] = 2*exp( -3.0*(dx*dx) ) + 0.1*alea.to<double>();
                A[1][i] = A[0][i];
            }
            if(fields.MPI.IsFirst)
            {
                A[0][inner.lower.x] = A[1][inner.lower.x] = A0(0);
            }
            if(fields.MPI.IsFinal)
            {
                A[0][inner.upper.x] = A[1][inner.upper.x] = 0;
            }
            fields.synchronize(A);
            tmx = 0;
        }

        void evolve()
        {
            field1D<double> &_A = A[0];
            field1D<double> &_L = L[0];

            tmx += 0.02;
            if(rank==0)
            {
                _A[inner.lower.x] = A0(tmx);
            }

            // compute inner laplacian
            for(coord1D i=inner.lower.x+1;i<inner.upper.x;++i)
            {
                _L[i] =  (_A[i-1]+_A[i+1]-(2*_A[i]));
            }

            // update
            for(coord1D i=inner.lower.x+1;i<inner.upper.x;++i)
            {
                _A[i] += 0.3 * _L[i];
            }



            // update
            for(coord1D i=inner.lower.x;i<=inner.upper.x;++i)
            {
                A[1][i] = A[0][i];
            }
            fields.synchronize(A);
        }


    private:
        YOCTO_DISABLE_COPY_AND_ASSIGN(Heat1D);
    };

    class Simulation : public VisIt::Simulation, public Heat1D
    {
    public:
        explicit Simulation(const VisIt &visit,
                            const size_t NX) :
        VisIt::Simulation(visit),
        Heat1D(MPI,NX)
        {
            addCommand("init", this, & Simulation::init, true);
        }

        virtual ~Simulation() throw()
        {
        }

        virtual void setMetaData(visit_handle &md)
        {
            (void)__visit::add_mesh(md,rmesh);
            (void)__visit::add_variable(md,rmesh,A);

        }

        virtual visit_handle getMesh(const int , const string &mesh_name)
        {
            if( "mesh" == mesh_name )
            {
                return __visit::get_mesh(rmesh);
            }
            return VISIT_INVALID_HANDLE;
        }


        // getting data
        virtual visit_handle getVariable(int,const string &id)
        {
            if(id=="A")
            {
                return __visit::get_variable(A);
            }

            return VISIT_INVALID_HANDLE;
        }


        void init(YOCTO_VISIT_SIMULATION_CALLBACK_PROTO)
        {
            initialize();
            runTime = 0;
            cycle   = 0;
            //update();
        }

        virtual void one_step()
        {
            evolve();
        }

    private:
        YOCTO_DISABLE_COPY_AND_ASSIGN(Simulation);
    };

}


YOCTO_UNIT_TEST_IMPL(heat1d)
{
    YOCTO_MPI_ENV();
    const string sim_name = "heat1d";
    const string sim_comm = "heat1d";
    //const string sim_gui  = "loop.ui";


    // prepare VisIt
    VisIt &visit = VisIt::Start(sim_name,
                                sim_comm,
                                MPI,
                                NULL,
                                false);


    // prepare args
    if(argc<=1)
    {
        throw exception("usage: %s NX", argv[0]);
    }
    const size_t NX = strconv::to<size_t>(argv[1],"NX");
    Simulation   sim(visit,NX);

    sim.loop();


}
YOCTO_UNIT_TEST_DONE()
