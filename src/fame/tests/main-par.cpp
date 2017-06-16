#include "yocto/fame/format/vtk.hpp"
#include "yocto/program.hpp"
#include "yocto/fame/mpi.hpp"

using namespace yocto;
using namespace fame;


static size_t ng = 2;

template <typename COORD>
static inline
void display_ghosts(const ghosts_of<COORD> &G )
{
    YOCTO_MPI_GET();
    for(size_t dim=0;dim<YOCTO_FAME_DIM_OF(COORD);++dim)
    {
        const ghosts &g    = G[dim];
        string        info = vformat("ghosts#%u: kind=%s:\n", unsigned(dim), g.kind_text() );
        if(g.prev)
        {
            info  += vformat("\t+prev@%d\n", int(g.prev->rank));
        }
        else
        {
            info  += vformat("\t!prev\n");
        }
        
        if(g.next)
        {
            info  += vformat("\t+next@%d\n", int(g.next->rank));
        }
        else
        {
            info  += vformat("\t!next\n");
        }
        MPI.Printf(stderr,"%s",info.c_str());
    }
}

#include "yocto/ios/ocstream.hpp"


template <typename T>
static inline void fill2d(  field2d<T> &F, const layout<coord2d> &full)
{
    for(coord1d y=F.outer.lower.y;y<=F.outer.upper.y;++y)
    {
        const bool out_y = (y<F.inner.lower.y||y>F.inner.upper.y);
        const double Y = ( double(y)-double(full.lower.y) ) / double(full.width.y-1);
        for(coord1d x=F.outer.lower.x;x<=F.outer.upper.x;++x)
        {
            const double X = ( double(x) - double(full.lower.x) ) / double(full.width.x-1);
            if(out_y || (x<F.inner.lower.x||x>F.inner.upper.x) )
            {
                F[y][x] = -(1+F.self.rank);
            }
            else
            {
                assert( F.inner.has( coord2d(x,y) ) );
                F[y][x] = T(cos(6*X)+sin(6*Y));
            }
        }
    }
}

template <typename T>
static inline
void fill3d( field3d<T> &F, const layout<coord3d> &full )
{
    F.ld( -(1+F.self.rank) );
    for(coord1d z=F.inner.lower.z;z<=F.inner.upper.z;++z)
    {
        const double dZ = (z-full.lower.z)/double(full.width.z-1) - 0.5;
        for(coord1d y=F.inner.lower.y;y<=F.inner.upper.y;++y)
        {
            const double dY = (y-full.lower.y)/double(full.width.y-1) - 0.5;
            for(coord1d x=F.inner.lower.x;x<=F.inner.upper.x;++x)
            {
                const double dX = (x-full.lower.x)/double(full.width.x-1) - 0.5;
                F[z][y][x] = exp(-0.5*( square_of(dX) + square_of(dY) + square_of(dZ) ));
            }
        }
    }
}

YOCTO_PROGRAM_START()
{
    YOCTO_MPI_ENV();
    
    {
        const layout<coord1d> full(1,32);
        mpi_ghosts<coord1d>   xch(MPI);
        
        {
            MPI.Printf0(stderr,"IS  periodic\n");
            const domain<coord1d> D(MPI.CommWorldRank,MPI.CommWorldSize,NULL,full,1);
            layouts<coord1d>      W(D,ng);
            ghosts_of<coord1d>    G(W);
            
            
            MPI.Printf(stderr,"is %2ld.%2ld: layout: inner=[%5ld:%5ld], outer=[%5ld:%5ld], color=%d\n",
                       long(D.full.size),long(D.self.rank),
                       long(W.inner.lower),
                       long(W.inner.upper),
                       long(W.outer.lower),
                       long(W.outer.upper),
                       int(W.color)
                       );
            
            //display_ghosts(G);
            
            
            
            
            field1d<float>             F("F",D,ng);
            F.ld(-(MPI.CommWorldRank+1));
            for(coord1d i=F.inner.lower;i<=F.inner.upper;++i)
            {
                F[i] = double(i) / double(full.upper);
            }
            
            xch.prepare_for(G,F);
            
            const string  output = vformat("1d_%d.%d.vtk",MPI.CommWorldSize,MPI.CommWorldRank);
            ios::wcstream fp(output);
            VTK::Header(fp, "field1d, periodic");
            VTK::StructuredPoints(fp,F.outer);
            VTK::SaveScalars(fp, F.name + "_ini", F, F.outer);
            xch.perform(G,F);
            VTK::SaveScalars(fp, F.name + "_end", F, F.outer);
            
        }
        
        
        if(false)
        {
            MPI.Printf0(stderr,"\nNOT  periodic\n");
            const domain<coord1d> D(MPI.CommWorldRank,MPI.CommWorldSize,NULL,full,0);
            layouts<coord1d>      W(D,ng);
            ghosts_of<coord1d>    G(W);
            
            
            
            MPI.Printf(stderr,"is %2ld.%2ld: layout: inner=[%5ld:%5ld], outer=[%5ld:%5ld], color=%d\n",
                       long(D.full.size),long(D.self.rank),
                       long(W.inner.lower),
                       long(W.inner.upper),
                       long(W.outer.lower),
                       long(W.outer.upper),
                       int(W.color)
                       );
            
            display_ghosts(G);
            field1d<double>     F("F",D,ng);
            xch.prepare_for(G,F);
            
            F.ld(-(MPI.CommWorldRank+1));
            for(coord1d i=F.inner.lower;i<=F.inner.upper;++i)
            {
                F[i] = double(i) / double(full.upper);
            }
            
            const string  output = vformat("1d_%d.%d_np.vtk",MPI.CommWorldSize,MPI.CommWorldRank);
            ios::wcstream fp(output);
            VTK::Header(fp, "field1d, not periodic");
            VTK::StructuredPoints(fp,F.outer);
            VTK::SaveScalars(fp, F.name + "_ini", F, F.outer);
            xch.perform(G,F);
            VTK::SaveScalars(fp, F.name + "_end", F, F.outer);
            
        }
        
    }
    
    if(false)
    {
        MPI.Printf0(stderr, "\n\n-------- IN 2D --------\n\n");
        {
            mpi_ghosts<coord2d>   xch(MPI);
            
            const layout<coord2d> full( coord2d(1,1), coord2d(32,32) );
            {
                MPI.Printf0(stderr,"IS  periodic XY\n");
                const domain<coord2d> D(MPI.CommWorldRank,MPI.CommWorldSize,NULL,full,coord2d(1,1));
                field2d<double>       F("F",D,ng);
                ghosts_of<coord2d>    G(F);
                display_ghosts(G);
                xch.prepare_for(G,F);
                
                
                fill2d(F,full);
                const string  output = vformat("2d_%d.%d_pbcXY.vtk",MPI.CommWorldSize,MPI.CommWorldRank);
                ios::wcstream fp(output);
                
                VTK::Header(fp, "field2d");
                VTK::StructuredPoints(fp,F.outer);
                
                VTK::SaveScalars(fp, F.name + "_ini", F, F.outer);
                xch.perform(G,F);
                VTK::SaveScalars(fp, F.name + "_end", F, F.outer);
                
            }
            
            {
                MPI.Printf0(stderr,"IS  periodic X\n");
                const domain<coord2d> D(MPI.CommWorldRank,MPI.CommWorldSize,NULL,full,coord2d(1,0));
                field2d<double>       F("F",D,ng);
                ghosts_of<coord2d>    G(F);
                display_ghosts(G);
                
                xch.prepare_for(G,F);
                
                fill2d(F,full);
                const string  output = vformat("2d_%d.%d_pbcX.vtk",MPI.CommWorldSize,MPI.CommWorldRank);
                ios::wcstream fp(output);
                
                VTK::Header(fp, "field2d");
                VTK::StructuredPoints(fp,F.outer);
                
                VTK::SaveScalars(fp, F.name + "_ini", F, F.outer);
                xch.perform(G,F);
                VTK::SaveScalars(fp, F.name + "_end", F, F.outer);
            }
            
            {
                MPI.Printf0(stderr,"IS  periodic Y\n");
                const domain<coord2d> D(MPI.CommWorldRank,MPI.CommWorldSize,NULL,full,coord2d(0,1));
                field2d<double>       F("F",D,ng);
                ghosts_of<coord2d>    G(F);
                display_ghosts(G);
                
                xch.prepare_for(G,F);
                
                fill2d(F,full);
                const string  output = vformat("2d_%d.%d_pbcY.vtk",MPI.CommWorldSize,MPI.CommWorldRank);
                ios::wcstream fp(output);
                
                VTK::Header(fp, "field2d");
                VTK::StructuredPoints(fp,F.outer);
                
                VTK::SaveScalars(fp, F.name + "_ini", F, F.outer);
                xch.perform(G,F);
                VTK::SaveScalars(fp, F.name + "_end", F, F.outer);
            }
            
            {
                MPI.Printf0(stderr,"IS  NOT periodic\n");
                const domain<coord2d> D(MPI.CommWorldRank,MPI.CommWorldSize,NULL,full,coord2d(0,0));
                field2d<double>       F("F",D,ng);
                ghosts_of<coord2d>    G(F);
                display_ghosts(G);
                
                xch.prepare_for(G,F);
                
                fill2d(F,full);
                const string  output = vformat("2d_%d.%d_no_pbc.vtk",MPI.CommWorldSize,MPI.CommWorldRank);
                ios::wcstream fp(output);
                
                VTK::Header(fp, "field2d");
                VTK::StructuredPoints(fp,F.outer);
                
                VTK::SaveScalars(fp, F.name + "_ini", F, F.outer);
                xch.perform(G,F);
                VTK::SaveScalars(fp, F.name + "_end", F, F.outer);
            }
            
            
            
        }
        
    }
    
    MPI.Printf0(stderr, "\n\n-------- IN 3D --------\n\n");
    {
        mpi_ghosts<coord3d>   xch(MPI);
        
        const layout<coord3d> full( coord3d(1,1,1), coord3d(32,32,32) );
        {
            MPI.Printf0(stderr,"IS  periodic XY\n");
            const domain<coord3d> D(MPI.CommWorldRank,MPI.CommWorldSize,NULL,full,coord3d(1,1,1));
            field3d<float>        F("F",D,ng);
            ghosts_of<coord3d>    G(F);
            //display_ghosts(G);
            xch.prepare_for(G,F);
            
            
            fill3d(F,full);
            const string  output = vformat("3d_%d.%d_XYZ.vtk",MPI.CommWorldSize,MPI.CommWorldRank);
            ios::wcstream fp(output);
            
            VTK::Header(fp, "field3d");
            VTK::StructuredPoints(fp,F.outer);
            
            VTK::SaveScalars(fp, F.name + "_ini", F, F.outer);
            xch.perform(G,F);
            VTK::SaveScalars(fp, F.name + "_end", F, F.outer);
            
        }
    }
    
}
YOCTO_PROGRAM_END()

