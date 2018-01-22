#include "yocto/ipso/mpi.hpp"
#include "yocto/program.hpp"
#include "yocto/string/conv.hpp"

using namespace yocto;
using namespace ipso;

template <typename COORD>
static inline
void mpi_xch( workspace<COORD> &W )
{
    const mpi &MPI = mpi::instance();

    field_info &A = W.fields["A"];
    field_info &B = W.fields["B"];

    W.sync_init_send();
    W.sync_store(A);
    W.sync_store(B);
    W.sync_init_recv();

    W.sync_query(A);
    W.sync_query(B);
}

YOCTO_PROGRAM_START()
{
    YOCTO_MPI_ENV();
    const int size = MPI.CommWorldSize;
    const int rank = MPI.CommWorldRank;
    MPI.Printf(stderr, "%s ready...\n",program);
    if(argc<=3)
    {
        throw exception("usage: %s DIMS PBCS GHOSTS", argv[0]);
    }
    const coord3D dims = __coord_parser::get<coord3D>(argv[1],"dims");
    const coord3D pbcs = __coord_parser::get<coord3D>(argv[2],"pbcs");
    const coord1D ng   = strconv::to_size(argv[3],"ng");

    if(0==rank)
    {
        fprintf(stderr,"dims="); __coord_printf(stderr,dims); fprintf(stderr,"\n");
        fprintf(stderr,"pbcs="); __coord_printf(stderr,pbcs); fprintf(stderr,"\n");
        fprintf(stderr,"ng  ="); __coord_printf(stderr,ng);   fprintf(stderr,"\n");
    }
    

    {
        MPI.Printf0(stderr, "\nin 1D\n" );
        // setup from MPI
        const patch1D region(1,dims.x);
        coord1D       fallback=0;
        coord1D       sizes = partition1D::optimal(size,ng,region,pbcs.x, &fallback, NULL);
        if( __coord_prod(sizes) < size )
        {
            MPI.Printf0(stderr,"switching to fallback\n");
            sizes = fallback;
        }
        if( __coord_prod(sizes) < size )
        {
            throw exception("unable to use %d cores", size);
        }
        if(0==rank)
        {
            fprintf(stderr,"sizes="); __coord_printf(stderr,sizes); fprintf(stderr,"\n");
        }

        // create divider
        divide::in1D           full(sizes,region);

        //create workspace
        mpi_workspace<coord1D> W(full,rank,ng,pbcs.x,32);
        field1D<double> &A = W.create< field1D<double> >("A");
        field1D<float>  &B = W.create< field1D<float>  >("B");

        // initialize fields
        A.ld(rank+1);
        B.ldz();

        // perform exchange
        mpi_xch(W);

    }


    {
        MPI.Printf0(stderr, "\nin 2D\n" );
        // setup from MPI
        const patch2D region(coord2D(1,1),dims.xy());
        coord2D       fallback;
        coord2D       sizes = partition2D::optimal(size,ng,region,pbcs.xy(), &fallback, NULL);
        if( __coord_prod(sizes) < size )
        {
            MPI.Printf0(stderr,"switching to fallback\n");
            sizes = fallback;
        }
        if( __coord_prod(sizes) < size )
        {
            throw exception("unable to use %d cores", size);
        }
        if(0==rank)
        {
            fprintf(stderr,"sizes="); __coord_printf(stderr,sizes); fprintf(stderr,"\n");
        }

        // create divider
        divide::in2D           full(sizes,region);

        //create workspace
        mpi_workspace<coord2D> W(full,rank,ng,pbcs.xy(),32);
        field2D<double> &A = W.create< field2D<double> >("A");
        field2D<float>  &B = W.create< field2D<float>  >("B");

        // initialize fields
        A.ld(rank+1);
        B.ldz();

        // perform exchange
        mpi_xch(W);

    }

    {
        MPI.Printf0(stderr, "\nin 3D\n" );
        // setup from MPI
        const patch3D region(coord3D(1,1,1),dims);
        coord3D       fallback;
        coord3D       sizes = partition3D::optimal(size,ng,region,pbcs, &fallback, NULL);
        if( __coord_prod(sizes) < size )
        {
            MPI.Printf0(stderr,"switching to fallback\n");
            sizes = fallback;
        }
        if( __coord_prod(sizes) < size )
        {
            throw exception("unable to use %d cores", size);
        }
        if(0==rank)
        {
            fprintf(stderr,"sizes="); __coord_printf(stderr,sizes); fprintf(stderr,"\n");
        }

        // create divider
        divide::in3D           full(sizes,region);

        //create workspace
        mpi_workspace<coord3D> W(full,rank,ng,pbcs,32);
        field3D<double> &A = W.create< field3D<double> >("A");
        field3D<float>  &B = W.create< field3D<float>  >("B");

        // initialize fields
        A.ld(rank+1);
        B.ldz();

        // perform exchange
        mpi_xch(W);

    }



}
YOCTO_PROGRAM_END()


