#include "yocto/fame/mpi.hpp"
#include "yocto/program.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/fame/format/vtk.hpp"

using namespace yocto;
using namespace fame;

YOCTO_PROGRAM_START()
{
    YOCTO_MPI_ENV();
    const coord1d ng = 2;

    {
        const layout<coord1d> full(1,32);
        mpi_ghosts<coord1d>   xch(MPI);

        // PBC
        {
            MPI.Printf0(stderr,"1D PBC\n");
            const domain<coord1d> D(MPI.CommWorldRank,MPI.CommWorldSize,NULL,full,1);


            fields iof;
            field1d<double> &F1 = iof.record( new field1d<double>("F1",D,ng) );
            field1d<float>  &F2 = iof.record( new field1d<float>("F2",D,ng) );

            ghosts_of<coord1d> G(F1);
            F1.ld(1+D.self.rank);
            F2.ld(1+D.self.rank);

            MPI.Printf(stderr, "\tblock_size=%u\n", unsigned(iof.block_size) );
            xch.prepare_for(G,iof);

            const string  filename = MPI.VTK_FileName("1d_pbc_");
            ios::wcstream fp(filename);
            VTK::Header(fp, "field1d, periodic");
            VTK::StructuredPoints(fp,F1.outer);
            VTK::SaveScalars(fp, F1.name + "_ini", F1, F1.outer);
            VTK::SaveScalars(fp, F2.name + "_ini", F2, F1.outer);

            xch.perform(G,iof);
            VTK::SaveScalars(fp, F1.name + "_end", F1, F1.outer);
            VTK::SaveScalars(fp, F2.name + "_end", F2, F1.outer);
        }

        
        {

        }
    }


    {
        const layout<coord2d> full(coord2d(1,1),coord2d(32,32));
        mpi_ghosts<coord2d>   xch(MPI);

        // PBC XY
        {
            MPI.Printf0(stderr,"2D PBC_XY\n");
            const domain<coord2d> D(MPI.CommWorldRank,MPI.CommWorldSize,NULL,full,coord2d(1,1));
            fields iof;
            field2d<double>    &F1 = iof.record( new field2d<double>("F1",D,ng) );
            field2d<float>     &F2 = iof.record( new field2d<float>("F2",D,ng) );
            ghosts_of<coord2d> G(F1);

            F1.ld(1+D.self.rank);
            F2.ld(1+D.self.rank);

            MPI.Printf(stderr, "\tblock_size=%u\n", unsigned(iof.block_size) );
            xch.prepare_for(G,iof);

            const string  filename = MPI.VTK_FileName("2d_xy_");
            ios::wcstream fp(filename);
            VTK::Header(fp, "field2d, periodic");
            VTK::StructuredPoints(fp,F1.outer);
            VTK::SaveScalars(fp, F1.name + "_ini", F1, F1.outer);
            VTK::SaveScalars(fp, F2.name + "_ini", F2, F1.outer);

            xch.perform(G,iof);
            VTK::SaveScalars(fp, F1.name + "_end", F1, F1.outer);
            VTK::SaveScalars(fp, F2.name + "_end", F2, F1.outer);


        }
    }
    
}
YOCTO_PROGRAM_END()
