#include "yocto/ipso/mpi.hpp"
#include "yocto/program.hpp"
#include "yocto/string/conv.hpp"

using namespace yocto;
using namespace ipso;

template <typename COORD>
static inline
void mpi_xch( mpi_workspace<COORD> &W )
{
    YOCTO_MPI_GET();

    field_info &A = W["A"];
    field_info &B = W["B"];

    fields fvar(2);
    fvar.append(A);
    fvar.append(B);

    W.sync_store_begin();
    //W.sync_store(A);
    // W.sync_store(B);
    W.sync_store(fvar);
    W.sync_store_end();

    string topology;
    for(size_t dim=0;dim<W.DIM;++dim)
    {
        topology << "| " << __coord_name(dim) << ":";
        const ghosts::list &gl = W.async[dim];
        topology << vformat(" #async=%u", unsigned(gl.size) );
        for(const ghosts *G=gl.head;G;G=G->next)
        {
            topology << vformat(" (%2u<->%2u@%s: %4u)", unsigned(G->source), unsigned(G->target), ghosts::pos2txt(G->pos), unsigned(G->count));
        }
        topology << " ";

    }
    MPI.Printf(stderr, "topology %s \n", *topology);
    MPI.Printf0(stderr,"perform exchange...\n");

    W.synchronize();
    W.sync_query(fvar);

#if 0
    for(size_t dim=0;dim<W.DIM;++dim)
    {
        const ghosts::list     &gl = W.async[dim];
        exchange_buffers::list &bl = W.iobuf[dim];
        assert(gl.size==bl.size);
        switch(gl.size)
        {

            case 1: {
                const ghosts     &g = *(gl.head);
                exchange_buffers &b = *(bl.head);
                assert( b.send.load() == b.recv.load() );
                W.sendrecv(MPI,b.send,g.target,b.recv,g.target);
            } break;

            case 2:{
                const ghosts     &g_lo = *(gl.head);
                const ghosts     &g_up = *(gl.tail);
                exchange_buffers &b_lo = *(bl.head);
                exchange_buffers &b_up = *(bl.tail);
                // up to lo
                W.sendrecv(MPI,b_up.send,g_up.target,b_lo.recv,g_lo.target);
                // to to up
                W.sendrecv(MPI,b_lo.send,g_lo.target,b_up.recv,g_up.target);
            } break;

            default:
                break;
        }
    }

    W.sync_query(A);
    W.sync_query(B);
#endif
}

YOCTO_PROGRAM_START()
{
    YOCTO_MPI(MPI_THREAD_SINGLE);
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
        fprintf(stderr,"\n-------- parameters --------\n");
        fprintf(stderr,"dims="); __coord_printf(stderr,dims); fprintf(stderr,"\n");
        fprintf(stderr,"pbcs="); __coord_printf(stderr,pbcs); fprintf(stderr,"\n");
        fprintf(stderr,"ng  ="); __coord_printf(stderr,ng);   fprintf(stderr,"\n");
    }
    

    if(true)
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
        mpi_workspace<coord1D> W(MPI,full,ng,pbcs.x,32);
        field1D<double> &A = W.create< field1D<double> >("A");
        field1D<float>  &B = W.create< field1D<float>  >("B");

        // initialize fields
        A.ld(rank+1);
        B.ldz();

        // perform exchange
        mpi_xch(W);

    }


    if(true)
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
        mpi_workspace<coord2D> W(MPI,full,ng,pbcs.xy(),32);
        field2D<double> &A = W.create< field2D<double> >("A");
        field2D<float>  &B = W.create< field2D<float>  >("B");

        // initialize fields
        A.ld(rank+1);
        B.ldz();

        // perform exchange
        mpi_xch(W);

    }

    if(true)
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
        mpi_workspace<coord3D> W(MPI,full,ng,pbcs,32);
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


