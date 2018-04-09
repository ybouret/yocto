#include "yocto/ipso/mpi.hpp"
#include "yocto/ipso/mapping.hpp"
#include "yocto/program.hpp"
#include "yocto/string/conv.hpp"
#include "yocto/ipso/format/vtk.hpp"
#include "yocto/ipso/box.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/fs/local-fs.hpp"
#include "yocto/code/alea.hpp"

using namespace yocto;
using namespace ipso;

static inline void write_topoly( const swaps_addr_list &asyncs, const int DIM )
{
    YOCTO_MPI_GET();
    string ans = vformat("Topology in %dD:\n",DIM);
    for(const swaps_addr_node *swp=asyncs.head;swp;swp=swp->next)
    {
        const swaps &s = **swp;
        ans += "\t";
        ans += swaps::flg2str(s.pos);
        ans += vformat(" %u->%u", unsigned(s.source), unsigned(s.target) );
        ans += "\n\t\t|_send:";
        for(size_t i=1;i<=s.send.size();++i)
        {
            ans += vformat(" %d", int(s.send[i]));
        }
        ans += "\n\t\t|_recv:";
        for(size_t i=1;i<=s.recv.size();++i)
        {
            ans += vformat(" %d", int(s.recv[i]));
        }
        ans += "\n";
    }
    MPI.Printf(stderr,"%s\n",*ans);
}

YOCTO_PROGRAM_START()
{
    YOCTO_MPI(MPI_THREAD_SINGLE);
    const int size = MPI.CommWorldSize;
    const int rank = MPI.CommWorldRank;
    alea.initialize();
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
        MPI.Printf0(stderr, "\n-------- setting up in 1D --------\n");
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
        write_topoly(W.asyncs,1);
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


    if( true )
    {
        MPI.Printf0(stderr, "\n-------- setting up in 2D --------\n");
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
        write_topoly(W.asyncs,2);
        field2D<double> &A = W.create< field2D<double> >("A");
        field2D<float>  &B = W.create< field2D<float>  >("B");

        fields fvar;
        fvar << A << B;
        W.allocate_swaps_for( fvar.block_size() );

        // initialize fields
        const double xmin = full.lower.x;
        const double xmax = full.upper.x;
        const double xc   = (xmax+xmin)/2;
        const double dx   = xmax-xmin;
        const double ymin = full.lower.y;
        const double ymax = full.upper.y;
        const double yc   = (ymax+ymin)/2;
        const double dy   = ymax-ymin;
        A.ldz();
        for(unit_t j=W.inner.lower.y;j<=W.inner.upper.y;++j)
        {
            const double y = (double(j)-yc)/dy;
            for(unit_t i=W.inner.lower.x;i<=W.inner.upper.x;++i)
            {
                const double x = (double(i)-xc)/dx;
                A[j][i] = 2+cos(13*math::Hypotenuse(x,y));
            }
        }
        B.ldz();

        {
            const string fn = "f2_ini" + MPI.CommWorldID + ".vtk";
            ios::wcstream fp(fn);
            VTK::InitSaveScalars(fp, "2D",A,A);
        }

        W.synchronize(fvar);

        {
            const string fn = "f2_end" + MPI.CommWorldID + ".vtk";
            ios::wcstream fp(fn);
            VTK::InitSaveScalars(fp, "2D",A,A);
        }

    }

    if( true )
    {
        MPI.Printf0(stderr, "\n-------- setting up in 3D --------\n");
        const patch3D region(coord3D(1,1,1),dims);
        coord3D       fallback;
        const coord3D PBCS(pbcs);
        coord3D       sizes = mapping<coord3D>::optimal_sizes_for(size,region,layers,PBCS,&fallback);
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

        divide::in3D           full(sizes,region);
        mpi_workspace<coord3D> W(MPI,full,layers,PBCS);
        MPI.Printf(stderr,"workspace @rank=%d, #items=%u\n", int(W.rank), unsigned(W.inner.items) );
        write_topoly(W.asyncs,3);
        field3D<double> &A = W.create< field3D<double> >("A");
        field3D<float>  &B = W.create< field3D<float>  >("B");

        fields fvar;
        fvar << A << B;
        W.allocate_swaps_for( fvar.block_size() );

        // initialize fields
        const double xmin = full.lower.x;
        const double xmax = full.upper.x;
        const double xc   = (xmax+xmin)/2;
        const double dx   = xmax-xmin;

        const double ymin = full.lower.y;
        const double ymax = full.upper.y;
        const double yc   = (ymax+ymin)/2;
        const double dy   = ymax-ymin;

        const double zmin = full.lower.z;
        const double zmax = full.upper.z;
        const double zc   = (zmax+zmin)/2;
        const double dz   = zmax-zmin;

        A.ldz();
        for(unit_t k=W.inner.lower.z;k<=W.inner.upper.z;++k)
        {
            const double z = (double(k)-zc)/dz;

            for(unit_t j=W.inner.lower.y;j<=W.inner.upper.y;++j)
            {
                const double y = (double(j)-yc)/dy;
                for(unit_t i=W.inner.lower.x;i<=W.inner.upper.x;++i)
                {
                    const double x = (double(i)-xc)/dx;
                    A[k][j][i] = 2+cos(12*sqrt(x*x+y*y+z*z));
                }
            }
        }
        B.ldz();

        {
            const string fn = "f3_ini" + MPI.CommWorldID + ".vtk";
            ios::wcstream fp(fn);
            VTK::InitSaveScalars(fp, "3D",A,A);
        }

        W.synchronize(fvar);

        {
            const string fn = "f3_end" + MPI.CommWorldID + ".vtk";
            ios::wcstream fp(fn);
            VTK::InitSaveScalars(fp, "3D",A,A);
        }

    }
    
}
YOCTO_PROGRAM_END()


