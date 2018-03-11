#include "yocto/ipso/mpi.hpp"
#include "yocto/program.hpp"
#include "yocto/string/conv.hpp"
#include "yocto/ipso/vtk.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/fs/local-fs.hpp"

using namespace yocto;
using namespace ipso;

static inline void write_topoly( const swaps_addr_list &asyncs )
{
    YOCTO_MPI_GET();
    string ans = "Topology:\n";
    for(const swaps_addr_node *swp=asyncs.head;swp;swp=swp->next)
    {
        const swaps &s = **swp;
        ans += "\t";
        ans += swaps::flg2str(s.pos);
        ans += vformat(" %u->%u", unsigned(s.source), unsigned(s.target) );
        ans += "\n";
    }
    MPI.Printf(stderr,"%s\n",*ans);
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
    // avoid removing newly created files!
    MPI.Barrier(MPI_COMM_WORLD);
    
    if(argc<=3)
    {
        throw exception("usage: %s DIMS PBCS LAYERS", argv[0]);
    }
    const coord3D dims   = __coord_parser::get<coord3D>(argv[1],"dims");
    const coord3D pbcs   = __coord_parser::get<coord3D>(argv[2],"pbcs");
    const size_t  layers = strconv::to_size(argv[3],"layers");

    if(0==rank)
    {
        fprintf(stderr,"\n-------- parameters --------\n");
        fprintf(stderr,"cpus   = %d\n",size);
        fprintf(stderr,"dims   = "); __coord_printf(stderr,dims);   fprintf(stderr,"\n");
        fprintf(stderr,"pbcs   = "); __coord_printf(stderr,pbcs);   fprintf(stderr,"\n");
        fprintf(stderr,"layers = %u\n", unsigned(layers));
    }

    if( true )
    {
        MPI.Printf0(stderr, "\nin 1D\n" );
        const patch1D region(1,dims.x);
        coord1D       fallback=0;
        const coord1D PBCS(pbcs.x);
        coord1D       sizes = mapping<coord1D>::optimal_sizes_for(size,region,layers,PBCS,&fallback);
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

        divide::in1D           full(sizes,region);
        mpi_workspace<coord1D> W(MPI,full,layers,PBCS);
        MPI.Printf(stderr,"workspace @rank=%d, #items=%u\n", int(W.rank), unsigned(W.inner.items) );
        write_topoly(W.asyncs);
        field1D<double> &A = W.create< field1D<double> >("A");
        field1D<float>  &B = W.create< field1D<float>  >("B");

        fields fvar;
        fvar << W["A"] << W["B"];

        W.allocate_swaps_for( fvar.block_size() );

        // initialize fields
        A.ldz();
        for(unit_t i=W.inner.lower;i<=W.inner.upper;++i)
        {
            A[i] = double(i);
            B[i] = -float(i);
        }

        {
            const string fn = "f1_ini" + MPI.CommWorldID + ".vtk";
            ios::wcstream fp(fn);
            VTK::InitSaveScalars(fp, "1D",A,A);
        }

        W.synchronize(fvar);

        {
            const string fn = "f1_end" + MPI.CommWorldID + ".vtk";
            ios::wcstream fp(fn);
            VTK::InitSaveScalars(fp, "1D",A,A);
        }
    }


    if( false )
    {
        MPI.Printf0(stderr, "\nin 2D\n" );
        const patch2D region(coord2D(1,1),dims.xy());
        coord2D       fallback;
        const coord2D PBCS(pbcs.xy());
        coord2D       sizes = mapping<coord2D>::optimal_sizes_for(size,region,layers,PBCS,&fallback);
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

        divide::in2D           full(sizes,region);
        mpi_workspace<coord2D> W(MPI,full,layers,PBCS);
        MPI.Printf(stderr,"workspace @rank=%d, #items=%u\n", int(W.rank), unsigned(W.inner.items) );
        field2D<double> &A = W.create< field2D<double> >("A");
        field2D<float>  &B = W.create< field2D<float>  >("B");

        fields fvar;
        fvar << A << B;

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


    }


#if 0
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
        A.ldz();
        for(unit_t k=W.inner.lower.z;k<=W.inner.upper.z;++k)
        {
            for(unit_t j=W.inner.lower.y;j<=W.inner.upper.y;++j)
            {
                for(unit_t i=W.inner.lower.x;i<=W.inner.upper.x;++i)
                {
                    A[k][j][i] = double(1+rank); //double(i*j);
                }
            }
        }
        B.ldz();
        {
            const string  vtk = MPI.VTK_FileName("in3D_init");
            ios::wcstream fp(vtk);
            VTK::InitSaveScalars(fp, "in2D", A, W.outer);
        }

        // perform exchange
        mpi_xch(W);
        {
            const string  vtk = MPI.VTK_FileName("in3D_sync");
            ios::wcstream fp(vtk);
            VTK::InitSaveScalars(fp, "in3D", A, W.outer);
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


#endif
    
}
YOCTO_PROGRAM_END()


