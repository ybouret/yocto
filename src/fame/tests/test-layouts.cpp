#include "yocto/fame/layouts.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace fame;

template <typename COORD>
static inline
void display_lay( const layouts_of<COORD> &L )
{
    std::cerr << L.full.sizes << "." << L.self.ranks << std::endl;
    std::cerr << "\tinner=" << L.inner << std::endl;
    std::cerr << "\touter=" << L.outer << std::endl;
}

YOCTO_UNIT_TEST_IMPL(layouts)
{
    {
        layout_of<coord1d> L(1,16);
        for(size_t size=1;size<=4;++size)
        {
            std::cerr << std::endl;
            std::cerr << "size=" << size << std::endl;
            
            std::cerr << "IS  periodic:" << std::endl;
            for(size_t rank=0;rank<size;++rank)
            {
                const domain_of<coord1d> D(rank,size,NULL,L,1);
                layouts_of<coord1d>      W(D,1);
                display_lay(W);
            }
            
            std::cerr << std::endl;
            std::cerr << "NOT periodic:" << std::endl;
            for(size_t rank=0;rank<size;++rank)
            {
                domain_of<coord1d>  D(rank,size,NULL,L,0);
                layouts_of<coord1d> W(D,1);
                display_lay(W);
            }
            
        }
    }
    
    
    {
        layout_of<coord2d> L( coord2d(1,1), coord2d(16,16) );
        for(size_t size=1;size<=4;++size)
        {
            std::cerr << std::endl;
            std::cerr << "size=" << size << std::endl;
            
            std::cerr << "IS  periodic:" << std::endl;
            for(size_t rank=0;rank<size;++rank)
            {
                domain_of<coord2d>  D(rank,size,NULL,L,coord2d(1,1));
                layouts_of<coord2d> W(D,1);
                display_lay(W);
            }
            
            std::cerr << "NOT periodic:" << std::endl;
            for(size_t rank=0;rank<size;++rank)
            {
                domain_of<coord2d> D(rank,size,NULL,L,coord2d(0,0));
                layouts_of<coord2d> W(D,1);
                display_lay(W);

            }
            
        }
    }

    {
        layout_of<coord3d> L( coord3d(1,1,1), coord3d(16,16,16) );
        for(size_t size=1;size<=8;++size)
        {
            std::cerr << std::endl;
            std::cerr << "size=" << size << std::endl;
            
            std::cerr << "IS  periodic:" << std::endl;
            for(size_t rank=0;rank<size;++rank)
            {
                domain_of<coord3d> D(rank,size,NULL,L,coord3d(1,1,1));
                layouts_of<coord3d> W(D,1);
                display_lay(W);
            }
            
            std::cerr << "NOT periodic:" << std::endl;
            for(size_t rank=0;rank<size;++rank)
            {
                domain_of<coord3d> D(rank,size,NULL,L,coord3d(0,0,0));
                layouts_of<coord3d> W(D,1);
                display_lay(W);
            }
            
        }
    }

    
    

}
YOCTO_UNIT_TEST_DONE()
