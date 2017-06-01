#include "yocto/fame/domain.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace fame;

YOCTO_UNIT_TEST_IMPL(domain)
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
                domain_of<coord1d> D(rank,size,NULL,L,1);
            }
            
            std::cerr << "NOT periodic:" << std::endl;
            for(size_t rank=0;rank<size;++rank)
            {
                domain_of<coord1d> D(rank,size,NULL,L,0);
            }
            
        }
    }
}
YOCTO_UNIT_TEST_DONE()
