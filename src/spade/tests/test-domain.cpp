#include "yocto/spade/domain.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace spade;

YOCTO_UNIT_TEST_IMPL(domain)
{
    const int smax = 5;
    
    {
        const Layout1D L(1,30);
        std::cerr << "full=" << L << std::endl;
        const Ghost1D  lower_ghost(-10,1);
        const Ghost1D  upper_ghost(10,2);
        const Ghosts1D G(-1,lower_ghost,upper_ghost);
        
        for(int size=1;size<=smax;++size)
        {
            std::cerr << "size=" << size << std::endl;
            for(int rank=0;rank<size;++rank)
            {
                {
                    DomainInfo<coord1D> D(L,rank,size);
                    std::cerr << "\t." << rank << "@" << D.ranks << "/" << D.sizes << ":" << D.part << std::endl;
                    Domain<double,coord1D> F(D,"F1",G);
                }
            }
        }
    }
    
    {
        const Layout2D L( coord2D(1,1), coord2D(30,30) );
        std::cerr << "full=" << L << std::endl;
        for(int size=1;size<=smax;++size)
        {
            std::cerr << "size=" << size << std::endl;
            for(int rank=0;rank<size;++rank)
            {
                {
                    DomainInfo<coord2D> D(L,rank,size);
                    std::cerr << "\t." << rank << "@" << D.ranks << "/" << D.sizes << ":" << D.part << std::endl;
                }
            }
        }
    }

    
}
YOCTO_UNIT_TEST_DONE()
