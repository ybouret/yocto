#include "yocto/fame/field3d.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace fame;

YOCTO_UNIT_TEST_IMPL(field)
{

    {
        const layout<coord1d> full(1,16);
        const domain<coord1d> dom0(0,1,NULL,full,1);
        field1d<int> Fi("Fi",dom0,0);
        field1d<int> FiCopy(Fi);
        for(coord1d x=Fi.outer.lower;x<=Fi.outer.upper;++x)
        {
            Fi[x] = x;
            Fi.at(x) = x;
        }
    }


    {
        const layout<coord2d> full( coord2d(1,1), coord2d(16,16) );
        {
            const domain<coord2d> dom0(0,1,NULL,full,coord2d(1,1));
            field2d<float> Ff("Ff",dom0,1);
            for(coord1d y=Ff.outer.lower.y;y<=Ff.outer.upper.y;++y)
            {
                for(coord1d x=Ff.outer.lower.x;x<=Ff.outer.upper.x;++x)
                {
                    const float f = x*y;
                    Ff[y][x] = f;
                    coord2d q(x,y);
                    if(Ff.at(q)<f||Ff.at(q)>f)
                    {
                        throw exception("invalid@(%d,%d)",int(x),int(y));
                    }
                }
            }
        }
        for(size_t size=1;size<=4;++size)
        {
            std::cerr << "\tsize=" << size << std::endl;
            for(size_t rank=0;rank<size;++rank)
            {
                std::cerr << "\t\trank=" << rank << std::endl;
                {
                    std::cerr << "\t\t\tPERIODIC:" << std::endl;
                    const domain<coord2d> dom(rank,size,NULL,full,coord2d(1,1));
                    field2d<char> F("F",dom,1);
                    std::cerr << "\t\t\tsizes=" << F.full.sizes << ".ranks=" << F.self.ranks << std::endl;
                    std::cerr << "\t\t\t2D inner=" << F.inner << std::endl;
                    std::cerr << "\t\t\t1D inner=" << F.first().inner << std::endl;
                    std::cerr << "\t\t\t2D outer=" << F.outer << std::endl;
                    std::cerr << "\t\t\t1D outer=" << F.first().outer << std::endl;

                }
            }
        }
        
    }
    
}
YOCTO_UNIT_TEST_DONE()
