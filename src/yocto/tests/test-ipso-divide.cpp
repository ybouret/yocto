#include "yocto/utest/run.hpp"
#include "yocto/ipso/domain.hpp"
#include "yocto/string/conv.hpp"
#include <cstdio>

using namespace yocto;
using namespace ipso;


YOCTO_UNIT_TEST_IMPL(ipso_divide)
{
    size_t Nx = 10;
    size_t Ny = 10;
    size_t Nz = 10;

    coord1D cpus = 4;

    if(argc>1)
    {
        cpus = strconv::to<size_t>(argv[1],"cpus");
    }

    {
        const patch1D full(1,Nx);
        std::cerr << "#CPU=" << cpus << " in 1D: " << full << std::endl;
        divide::in1D D(cpus,full);
        std::cerr << "Periodic:\n" << std::endl;
        std::cerr.flush();
        for(size_t rank=0;rank<D.size;++rank)
        {
            //std::cerr << '\t' << D.get_ranks(rank) << std::endl;
            //const patch1D sub = D(rank,NULL);
            //std::cerr << "\t\t" << sub << std::endl;
            domain<coord1D> dom(D,rank,1,1);
        }
        fflush(stderr);
        std::cerr << std::endl;


        std::cerr << "NOT Periodic:\n" << std::endl;
        std::cerr.flush();
        for(size_t rank=0;rank<D.size;++rank)
        {
            //std::cerr << '\t' << D.get_ranks(rank) << std::endl;
            //const patch1D sub = D(rank,NULL);
            //std::cerr << "\t\t" << sub << std::endl;
            domain<coord1D> dom(D,rank,1,0);
        }
        fflush(stderr);
        std::cerr << std::endl;

    }



    return 0;

    {

        const patch2D full( coord2D(1,1),coord2D(Nx,Ny) );
        std::cerr << "#CPU=" << cpus << " in 2D: " << full << std::endl;

        coord2D       sizes;
        for(sizes.x=1;sizes.x<=cpus;++sizes.x)
        {
            for(sizes.y=1;sizes.y<=cpus;++sizes.y)
            {
                if(sizes.__prod()!=cpus) continue;
                std::cerr << "sizes=" << sizes << std::endl;
                divide::in2D D(sizes,full);
                for(size_t rank=0;rank<D.size;++rank)
                {
                    std::cerr << '\t' << D.get_ranks(rank) << std::endl;
                    const patch2D sub = D(rank,NULL);
                    std::cerr << "\t\t" << sub << std::endl;
                }
            }
        }

    }

    {
        const patch3D full( coord3D(1,1,1),coord3D(Nx,Ny,Nz) );
        std::cerr << "#CPU=" << cpus << " in 3D: " << full << std::endl;
        coord3D       sizes;
        for(sizes.x=1;sizes.x<=cpus;++sizes.x)
        {
            for(sizes.y=1;sizes.y<=cpus;++sizes.y)
            {
                for(sizes.z=1;sizes.z<=cpus;++sizes.z)
                {
                    if(sizes.__prod()!=cpus) continue;
                    std::cerr << "sizes=" << sizes << std::endl;
                    divide::in3D D(sizes,full);
                    for(size_t rank=0;rank<D.size;++rank)
                    {
                        std::cerr << '\t' << D.get_ranks(rank) << std::endl;
                        const patch3D sub = D(rank,NULL);
                        std::cerr << "\t\t" << sub << std::endl;
                    }
                }
            }
        }

    }


}
YOCTO_UNIT_TEST_DONE()

