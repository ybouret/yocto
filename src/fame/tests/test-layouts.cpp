#include "yocto/fame/layouts.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace fame;

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
            }
            
            std::cerr << std::endl;
            std::cerr << "NOT periodic:" << std::endl;
            for(size_t rank=0;rank<size;++rank)
            {
                domain_of<coord1d>  D(rank,size,NULL,L,0);
                layouts_of<coord1d> W(D,1);
            }
            
        }
    }

}
YOCTO_UNIT_TEST_DONE()
