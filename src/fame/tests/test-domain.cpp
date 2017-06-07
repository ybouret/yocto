#include "yocto/fame/domain.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace fame;

#define __CHECK_HASH(MSG) do {\
H.set(); D.run_hash(H); const uint64_t  h0 = H.key<uint64_t>();\
H.set(); Db.run_hash(H); const uint64_t h1 = H.key<uint64_t>();\
if(h0!=h1)\
throw exception("invalid hash " #MSG );\
} while(false)

template <typename COORD>
static inline
void display_topo( const domain<COORD> &D )
{
    std::cerr << D.full.sizes << "." << D.self.ranks << std::endl;
    
    for(size_t dim=0;dim<YOCTO_FAME_DIM_OF(COORD);++dim)
    {
        std::cerr << "\tdim#" << dim << " : ";
        const typename domain<COORD>::link &lnk = D.links[dim];
        if(lnk.prev)
        {
            std::cerr << ((lnk.prev->local) ? "." : "+");
            std::cerr << "prev@" << lnk.prev->ranks;
        }
        else
        {
            std::cerr << "!prev";
        }
        std::cerr << " ";
        if(lnk.next)
        {
            std::cerr << ((lnk.next->local) ? "." : "+");
            std::cerr << "next@" << lnk.next->ranks;
        }
        else
        {
            std::cerr << "!next";
        }
        std::cerr << std::endl;
    }
    
}

YOCTO_UNIT_TEST_IMPL(domain)
{
    hashing::sha1 H;
    
    {
        layout<coord1d> L(1,16);
        for(size_t size=1;size<=4;++size)
        {
            std::cerr << std::endl;
            std::cerr << "size=" << size << std::endl;
            
            std::cerr << "IS  periodic:" << std::endl;
            for(size_t rank=0;rank<size;++rank)
            {
                domain<coord1d> D(rank,size,NULL,L,1);
                domain<coord1d> Db(D);
                
                __CHECK_HASH(1D pbc);
                display_topo(D);
            }
            
            std::cerr << std::endl;
            std::cerr << "NOT periodic:" << std::endl;
            for(size_t rank=0;rank<size;++rank)
            {
                domain<coord1d> D(rank,size,NULL,L,0);
                domain<coord1d> Db(D);
                __CHECK_HASH(1D no pbc);
                display_topo(D);
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
                domain<coord2d> D(rank,size,NULL,L,coord2d(1,1));
                domain<coord2d> Db(D);  __CHECK_HASH(2D pbc);
                display_topo(D);
            }
            
            std::cerr << "NOT periodic:" << std::endl;
            for(size_t rank=0;rank<size;++rank)
            {
                domain<coord2d> D(rank,size,NULL,L,coord2d(0,0));
                domain<coord2d> Db(D);__CHECK_HASH(2D no pbc);
                display_topo(D);
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
                domain<coord3d> Db(D);
                __CHECK_HASH(3D pbc);
                display_topo(D);

            }
            
            std::cerr << "NOT periodic:" << std::endl;
            for(size_t rank=0;rank<size;++rank)
            {
                domain<coord3d> D(rank,size,NULL,L,coord3d(0,0,0));
                domain<coord3d> Db(D);
                __CHECK_HASH(3D no pbc);
                display_topo(D);

            }
            
        }
    }

    
    
    
}
YOCTO_UNIT_TEST_DONE()
