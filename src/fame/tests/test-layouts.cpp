#include "yocto/fame/layouts.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace fame;

template <typename COORD>
static inline
void display_lay( const layouts<COORD> &L )
{
    std::cerr << L.full.sizes << "." << L.self.ranks << std::endl;
    std::cerr << "\tinner=" << L.inner << std::endl;
    std::cerr << "\touter=" << L.outer << std::endl;
    std::cerr << "\t\tminRealIndices:"; for(size_t i=0;i<YOCTO_FAME_DIM_OF(COORD);++i) { std::cerr << " " << L.imin[i]; } std::cerr << std::endl;
    std::cerr << "\t\tmaxRealIndices:"; for(size_t i=0;i<YOCTO_FAME_DIM_OF(COORD);++i) { std::cerr << " " << L.imax[i]; } std::cerr << std::endl;

}


static inline void display_ghost(const ghosts_pair &gp)
{
    std::cerr << "\t\touter@["; for(size_t i=1;i<=gp.size;++i) std::cerr << " " << gp.outer[i]; std::cerr << " ]" << std::endl;
    std::cerr << "\t\tinner@["; for(size_t i=1;i<=gp.size;++i) std::cerr << " " << gp.inner[i]; std::cerr << " ]" << std::endl;

}

template <typename COORD>
static inline
void display_ghosts(const ghosts_of<COORD> &G )
{
    
    for(size_t dim=0;dim<YOCTO_FAME_DIM_OF(COORD);++dim)
    {
        const ghosts &g = G[dim];
        std::cerr << "ghosts" << dim << ", kind=" << g.kind_text() << std::endl;

        if(g.prev)
        {
            std::cerr << "\tprev#" << g.prev->size << std::endl;
            display_ghost(*g.prev);
        }
        else
        {
            std::cerr << "\t!prev" << std::endl;
        }

        if(g.next)
        {
            std::cerr << "\tnext#" << g.next->size << std::endl;
            display_ghost(*g.next);
        }
        else
        {
            std::cerr << "\t!next" << std::endl;
        }
    }
}


YOCTO_UNIT_TEST_IMPL(layouts)
{

    {
        layout<coord1d> L(1,16);
        for(size_t size=1;size<=4;++size)
        {
            std::cerr << std::endl;
            std::cerr << "size=" << size << std::endl;
            
            std::cerr << "IS  periodic:" << std::endl;
            for(size_t rank=0;rank<size;++rank)
            {
                const domain<coord1d> D(rank,size,NULL,L,1);
                layouts<coord1d>      W(D,1);
                display_lay(W);
                ghosts_of<coord1d> G(W);
                display_ghosts(G);

            }
            
            std::cerr << std::endl;
            std::cerr << "NOT periodic:" << std::endl;
            for(size_t rank=0;rank<size;++rank)
            {
                domain<coord1d>  D(rank,size,NULL,L,0);
                layouts<coord1d> W(D,1);
                display_lay(W);
                ghosts_of<coord1d> G(W);
                display_ghosts(G);
            }

        }
    }


    {
        layout<coord2d> L( coord2d(1,1), coord2d(16,16) );
        for(size_t size=1;size<=4;++size)
        {
            std::cerr << std::endl;
            std::cerr << "size=" << size << std::endl;
            
            std::cerr << "IS  periodic:" << std::endl;
            for(size_t rank=0;rank<size;++rank)
            {
                domain<coord2d>  D(rank,size,NULL,L,coord2d(1,1));
                layouts<coord2d> W(D,1);
                display_lay(W);
                ghosts_of<coord2d> G(W);
                display_ghosts(G);
            }
            
            std::cerr << "NOT periodic:" << std::endl;
            for(size_t rank=0;rank<size;++rank)
            {
                domain<coord2d> D(rank,size,NULL,L,coord2d(0,0));
                layouts<coord2d> W(D,1);
                display_lay(W);
                ghosts_of<coord2d> G(W);
                display_ghosts(G);
            }

        }
    }


    {
        layout<coord3d> L( coord3d(1,1,1), coord3d(16,16,16) );
        for(size_t size=1;size<=8;++size)
        {
            std::cerr << std::endl;
            std::cerr << "size=" << size << std::endl;
            
            std::cerr << "IS  periodic:" << std::endl;
            for(size_t rank=0;rank<size;++rank)
            {
                domain<coord3d> D(rank,size,NULL,L,coord3d(1,1,1));
                layouts<coord3d> W(D,1);
                display_lay(W);
                ghosts_of<coord3d> G(W);
            }
            
            std::cerr << "NOT periodic:" << std::endl;
            for(size_t rank=0;rank<size;++rank)
            {
                domain<coord3d> D(rank,size,NULL,L,coord3d(0,0,0));
                layouts<coord3d> W(D,1);
                display_lay(W);
                //ghosts_of<coord3d> G(W);
            }

        }
    }

    
    

}
YOCTO_UNIT_TEST_DONE()
