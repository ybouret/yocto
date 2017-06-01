#include "yocto/fame/split.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace fame;

YOCTO_UNIT_TEST_IMPL(split)
{
    {
        layout_of<coord1d> L(1,16);
        for(size_t np=1;np<=16;++np)
        {
            coord1d sizes = np;
            split<coord1d> S(L,np,&sizes);
            for(size_t r=0;r<np;++r)
            {
                const layout_of<coord1d> sub = S(r);
                std::cerr << np << "." << r << ":" << sub << std::endl;
            }
            for(size_t rank=0;rank<np;++rank)
            {
                const coord1d ranks = S.local_ranks(rank);
                const coord1d rr    = S.global_rank(ranks);
                std::cerr << rank << "->" << ranks << "->" << rr << std::endl;
            }
            std::cerr << "Auto Split..." << std::endl;
            split<coord1d> AS(L,np,NULL);
            std::cerr << " => " << AS.sizes << std::endl;
            std::cerr << std::endl;
        }
    }
    
    {
        layout_of<coord2d> L( coord2d(1,1), coord2d(16,16) );
        for(size_t np=1;np<=16;++np)
        {
            for(size_t sx=1;sx<=np;++sx)
            {
                for(size_t sy=1;sy<=np;++sy)
                {
                    if(sx*sy!=np) continue;
                    const coord2d sizes(sx,sy);
                    std::cerr << "SIZES="<<sizes<<std::endl;
                    split<coord2d> S(L,np,&sizes);
                    for(size_t r=0;r<np;++r)
                    {
                        const layout_of<coord2d> sub = S(r);
                        std::cerr << np << "." << r << ":" << sub << std::endl;
                    }
                    
                    for(size_t rank=0;rank<np;++rank)
                    {
                        const coord2d ranks = S.local_ranks(rank);
                        const coord1d rr    = S.global_rank(ranks);
                        std::cerr << rank << "->" << ranks << "->" << rr << std::endl;
                    }
                    
                }
            }
            std::cerr << "Auto Split..." << std::endl;
            split<coord2d> AS(L,np);
            std::cerr << " => " << AS.sizes << std::endl;
            std::cerr << std::endl;
        }
    }
    
    {
        layout_of<coord3d> L( coord3d(1,1,1), coord3d(16,16,16) );
        for(size_t np=1;np<=16;++np)
        {
            for(size_t sx=1;sx<=np;++sx)
            {
                for(size_t sy=1;sy<=np;++sy)
                {
                    for(size_t sz=1;sz<=np;++sz)
                    {
                        if(sx*sy*sz!=np) continue;
                        const coord3d sizes(sx,sy,sz);
                        std::cerr << "SIZES="<<sizes<<std::endl;
                        split<coord3d> S(L,np,&sizes);
                        for(size_t r=0;r<np;++r)
                        {
                            const layout_of<coord3d> sub = S(r);
                            std::cerr << np << "." << r << ":" << sub << std::endl;
                        }
                        
                        for(size_t rank=0;rank<np;++rank)
                        {
                            const coord3d ranks = S.local_ranks(rank);
                            const coord1d rr    = S.global_rank(ranks);
                            std::cerr << rank << "->" << ranks << "->" << rr << std::endl;
                        }
                        
                    }
                }
            }
            std::cerr << "Auto Split..." << std::endl;
            split<coord3d> AS(L,np);
            std::cerr << " => " << AS.sizes << std::endl;
            std::cerr << std::endl;
        }
    }
    
    
    
    
    
}
YOCTO_UNIT_TEST_DONE()
