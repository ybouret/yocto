#include "yocto/spade/split.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace spade;

YOCTO_UNIT_TEST_IMPL(split)
{
    {
        Layout1D full(1,100);
        std::cerr << "full=" << full << std::endl;
        for(size_t np=1;np<=8;++np)
        {
            std::cerr << "NP=" << np << std::endl;
            const Split::In1D in1D(full,np);
            for(size_t r=0;r<np;++r)
            {
                const Layout1D sub = in1D(r);
                std::cerr << "\tsub#" << r << "=" << sub << std::endl;
            }
            for(size_t r=0;r<np;++r)
            {
                
                std::cerr << "\t\t" << in1D.prev(r,0) << "->" << r << "->" << in1D.next(r,0) << std::endl;
            }
        }
    }
    
    

    {
        Layout2D full( coord2D(1,1), coord2D(100,100) );
        std::cerr << "full=" << full << std::endl;
        for(size_t nx=1;nx<=4;++nx)
        {
            for(size_t ny=1;ny<=4;++ny)
            {

                const size_t nn = nx*ny;
                std::cerr << "nx=" << nx << ", ny=" << ny << ", nn=" << nn << std::endl;
                const Split::In2D in2D(full,coord2D(nx,ny));
                for(size_t r=0;r<nn;++r)
                {
                    const Layout2D sub = in2D(r);
                    std::cerr << "\tsub#" << r << "=" << sub << std::endl;
                }
                
                for(size_t r=0;r<nn;++r)
                {
                    const coord2D ranks = in2D.getRanks(r);
                    std::cerr << "rank=" << r << " -> ranks=" << ranks << std::endl;
                    for(size_t dim=0;dim<2;++dim)
                    {
                        const coord2D prev = in2D.prev(ranks,dim);
                        const coord2D next = in2D.next(ranks,dim);
                        std::cerr << "\tdim" << dim << ": " << prev << "->" << ranks << "->" << next << std::endl;
                    }
                }
            }
        }
    }
    

    {
        Layout3D full( coord3D(1,1,1), coord3D(100,100,100) );
        std::cerr << "full=" << full << std::endl;
        for(size_t nx=1;nx<=4;++nx)
        {
            for(size_t ny=1;ny<=4;++ny)
            {
                for(size_t nz=1;nz<=4;++nz)
                {
                    const size_t nn = nx*ny*nz;
                    std::cerr << "nx=" << nx << ", ny=" << ny << ", nz=" << nz << ", nn=" << nn << std::endl;
                    const Split::In3D in3D(full,coord3D(nx,ny,nz));
                    for(size_t r=0;r<nn;++r)
                    {
                        const coord3D ranks = in3D.getRanks(r);
                        std::cerr << "\trank=" << r << " => " << ranks << std::endl;
                        const Layout3D sub = in3D(r);
                        std::cerr << "\tsub#" << r << "=" << sub << std::endl;
                    }
                    
                    for(size_t r=0;r<nn;++r)
                    {
                        const coord3D ranks = in3D.getRanks(r);
                        std::cerr << "rank=" << r << " -> ranks=" << ranks << std::endl;
                        for(size_t dim=0;dim<3;++dim)
                        {
                            const coord3D prev = in3D.prev(ranks,dim);
                            const coord3D next = in3D.next(ranks,dim);
                            std::cerr << "\tdim" << dim << ": " << prev << "->" << ranks << "->" << next << std::endl;
                        }
                    }

                    
                }
            }
        }
    }
    
}
YOCTO_UNIT_TEST_DONE()
