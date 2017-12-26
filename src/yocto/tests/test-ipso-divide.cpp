#include "yocto/utest/run.hpp"
#include "yocto/ipso/divide.hpp"
#include "yocto/string/conv.hpp"

using namespace yocto;
using namespace ipso;


YOCTO_UNIT_TEST_IMPL(ipso_divide)
{
    size_t Nx = 10;
    size_t Ny = 10;
    size_t Nz = 10;

    size_t cpus = 4;

    if(argc>1)
    {
        cpus = strconv::to<size_t>(argv[1],"cpus");
    }

    {
        std::cerr << "#CPU=" << cpus << " in 1D" << std::endl;
        const patch1D full(1,Nx);
        divide::in1D D(cpus,full);
        for(size_t rank=0;rank<D.size;++rank)
        {
            std::cerr << '\t' << D.get_ranks(rank) << std::endl;
        }

    }


    {
        std::cerr << "#CPU=" << cpus << " in 2D" << std::endl;

        const patch2D full( coord2D(1,1),coord2D(Nx,Ny) );
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
                }
            }
        }

    }

    {
        std::cerr << "#CPU=" << cpus << " in 3D" << std::endl;

        const patch3D full( coord3D(1,1,1),coord3D(Nx,Ny,Nz) );
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
                    }
                }
            }
        }

    }


}
YOCTO_UNIT_TEST_DONE()

