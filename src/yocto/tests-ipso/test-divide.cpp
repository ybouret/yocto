#include "yocto/ipso/divide.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/string/conv.hpp"
#include <cstdio>

using namespace yocto;
using namespace ipso;

void test_rank_conversion(const coord1D n)
{
    const coord1D Nx = n;
    const coord1D Ny = n;
    const coord1D Nz = n;

    {
        const patch1D full(1,Nx);
        for(coord1D sizes=1;sizes<=Nx;++sizes)
        {
            divide::in1D dv(sizes,full);
            for(size_t rank=0;rank<dv.size;++rank)
            {
                const coord1D ranks = dv.get_ranks(rank);
                if( dv.get_rank_from(ranks) != rank )
                {
                    throw exception("rank mismatch 1D");
                }
            }
        }
    }

    {
        const patch2D full( coord2D(1,1),coord2D(Nx,Ny) );
        coord2D       sizes;
        for(sizes.x=1;sizes.x<=Nx;++sizes.x)
        {
            for(sizes.y=1;sizes.y<=Ny;++sizes.y)
            {
                divide::in2D dv(sizes,full);
                for(size_t rank=0;rank<dv.size;++rank)
                {
                    const coord2D ranks = dv.get_ranks(rank);
                    if( dv.get_rank_from(ranks) != rank )
                    {
                        throw exception("rank mismatch 2D");
                    }
                }
            }
        }
    }

    {
        const patch3D full( coord3D(1,1,1),coord3D(Nx,Ny,Nz) );
        coord3D       sizes;
        for(sizes.x=1;sizes.x<=Nx;++sizes.x)
        {
            for(sizes.y=1;sizes.y<=Ny;++sizes.y)
            {
                for(sizes.z=1;sizes.z<=Nz;++sizes.z)
                {
                    divide::in3D dv(sizes,full);
                    for(size_t rank=0;rank<dv.size;++rank)
                    {
                        const coord3D ranks = dv.get_ranks(rank);
                        if( dv.get_rank_from(ranks) != rank )
                        {
                            throw exception("rank mismatch 3D");
                        }
                    }
                }
            }
        }
    }

}





YOCTO_UNIT_TEST_IMPL(divide)
{
    test_rank_conversion(10);
}
YOCTO_UNIT_TEST_DONE()

