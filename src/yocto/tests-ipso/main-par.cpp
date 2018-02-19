#include "yocto/ipso/mpi.hpp"
#include "yocto/program.hpp"
#include "yocto/string/conv.hpp"
#include "yocto/ipso/vtk.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/fs/local-fs.hpp"

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

}

YOCTO_PROGRAM_START()
{
    YOCTO_MPI(MPI_THREAD_SINGLE);
    const int size = MPI.CommWorldSize;
    const int rank = MPI.CommWorldRank;
    MPI.Printf(stderr, "%s ready...\n",program);
    if(0==rank)
    {
        local_fs & fs = local_fs::instance();
        fs.remove_files_with_extension_in("./", "vtk");
    }
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
        coord1D       sizes = divider<coord1D>::optimal_for(region,size,&fallback);
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
        for(unit_t i=W.inner.lower;i<=W.inner.upper;++i)
        {
            A[i] = double(i);
            B[i] = -float(i);
        }

        {
            const string  vtk = MPI.VTK_FileName("in1D_init");
            ios::wcstream fp(vtk);
            VTK::InitSaveScalars(fp, "in1D", A, W.outer);
            VTK::SaveScalars(fp, B, W.outer);
        }

        // perform exchange
        mpi_xch(W);

        {
            const string  vtk = MPI.VTK_FileName("in1D_sync");
            ios::wcstream fp(vtk);
            VTK::InitSaveScalars(fp, "in1D", A, W.outer);
            VTK::SaveScalars(fp, B, W.outer);
        }

        fields fvar(2);
        fvar.append(A);
        fvar.append(B);

        MPI.Printf0(stderr,"sync '%s' and '%s'\n", * A.name, *B.name );
        W.synchronize(A);
        W.synchronize(B);

        MPI.Printf0(stderr,"sync fields\n");
        W.synchronize(fvar);

    }


    if(true)
    {
        MPI.Printf0(stderr, "\nin 2D\n" );
        // setup from MPI
        const patch2D region(coord2D(1,1),dims.xy());
        coord2D       fallback;
        coord2D       sizes = divider<coord2D>::optimal_for(region,size,&fallback);
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
        A.ldz();
        for(unit_t j=W.inner.lower.y;j<=W.inner.upper.y;++j)
        {
            for(unit_t i=W.inner.lower.x;i<=W.inner.upper.x;++i)
            {
                A[j][i] = double(1+rank); //double(i*j);
            }
        }

        B.ldz();

        {
            const string  vtk = MPI.VTK_FileName("in2D_init");
            ios::wcstream fp(vtk);
            VTK::InitSaveScalars(fp, "in2D", A, W.outer);
        }

        // perform exchange
        mpi_xch(W);

        {
            const string  vtk = MPI.VTK_FileName("in2D_sync");
            ios::wcstream fp(vtk);
            VTK::InitSaveScalars(fp, "in2D", A, W.outer);
        }

        fields fvar(2);
        fvar.append(A);
        fvar.append(B);

        MPI.Printf0(stderr,"sync '%s' and '%s'\n", * A.name, *B.name );
        W.synchronize(A);
        W.synchronize(B);

        MPI.Printf0(stderr,"sync fields\n");
        W.synchronize(fvar);

    }

    if(true)
    {
        MPI.Printf0(stderr, "\nin 3D\n" );
        // setup from MPI
        const patch3D region(coord3D(1,1,1),dims);
        coord3D       fallback;
        coord3D       sizes = divider<coord3D>::optimal_for(region,size,&fallback);
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

        fields fvar(2);
        fvar.append(A);
        fvar.append(B);

        MPI.Printf0(stderr,"sync '%s' and '%s'\n", * A.name, *B.name );
        W.synchronize(A);
        W.synchronize(B);

        MPI.Printf0(stderr,"sync fields\n");
        W.synchronize(fvar);

    }



}
YOCTO_PROGRAM_END()


