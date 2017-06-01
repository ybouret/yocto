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
                domain_of<coord1d> Db(D);
            }
            
            std::cerr << std::endl;
            std::cerr << "NOT periodic:" << std::endl;
            for(size_t rank=0;rank<size;++rank)
            {
                domain_of<coord1d> D(rank,size,NULL,L,0);
                domain_of<coord1d> Db(D);
            }
            
        }
    }
    //return 0;
    
    {
        layout_of<coord2d> L( coord2d(1,1), coord2d(16,16) );
        for(size_t size=1;size<=4;++size)
        {
            std::cerr << std::endl;
            std::cerr << "size=" << size << std::endl;
            
            std::cerr << "IS  periodic:" << std::endl;
            for(size_t rank=0;rank<size;++rank)
            {
                domain_of<coord2d> D(rank,size,NULL,L,coord2d(1,1));
                domain_of<coord2d> Db(D);

            }
            
            std::cerr << "NOT periodic:" << std::endl;
            for(size_t rank=0;rank<size;++rank)
            {
                domain_of<coord2d> D(rank,size,NULL,L,coord2d(0,0));
                domain_of<coord2d> Db(D);
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
                domain_of<coord3d> Db(D);
            }
            
            std::cerr << "NOT periodic:" << std::endl;
            for(size_t rank=0;rank<size;++rank)
            {
                domain_of<coord3d> D(rank,size,NULL,L,coord3d(0,0,0));
                domain_of<coord3d> Db(D);
            }
            
        }
    }

    
    
    
}
YOCTO_UNIT_TEST_DONE()
