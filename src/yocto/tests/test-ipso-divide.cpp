#include "yocto/utest/run.hpp"
#include "yocto/ipso/domain.hpp"
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

template <typename COORD>
static inline
void display( const typename domain<COORD>::list &L )
{
    for(const domain<COORD> *d = L.head; d; d=d->next)
    {
        std::cerr << "domain " << d->ranks << std::endl;
        std::cerr << "\tinner  : " << d->inner << std::endl;
        std::cerr << "\touter  : " << d->outer << std::endl;
        continue;
        std::cerr << "\t#async : " << d->async.size << std::endl;
        for(const ghosts *g=d->async.head;g;g=g->next)
        {
            std::cerr << "\t\tsend: " << g->source << "->" << g->target << "@" << g->send << std::endl;
            std::cerr << "\t\trecv: " << g->source << "<-" << g->target << "@" << g->recv << std::endl;
        }
        std::cerr << "\t#local : " << d->local.size << std::endl;
        for(const ghosts *g=d->local.head;g;g=g->next)
        {
            std::cerr << "\t\tsend: " << g->source << "->" << g->target << "@" << g->send << std::endl;
            std::cerr << "\t\trecv: " << g->source << "<-" << g->target << "@" << g->recv << std::endl;
        }

    }
}

YOCTO_UNIT_TEST_IMPL(ipso_divide)
{
    test_rank_conversion(10);

    size_t Nx = 10;
    size_t Ny = 10;
    size_t Nz = 10;

    coord1D cpus = 4;
    coord1D ng   = 1;

    if(argc>1)
    {
        cpus = strconv::to<size_t>(argv[1],"cpus");
    }

    if(argc>2)
    {
        ng = strconv::to<size_t>(argv[2],"ng");
    }

    {
        const patch1D full(1,Nx);
        std::cerr << "#CPU=" << cpus << " in 1D: " << full << std::endl;
        divide::in1D D(cpus,full);
        std::cerr << "Periodic:" << std::endl;
        std::cerr.flush();
        domain<coord1D>::list p_domains;
        for(size_t rank=0;rank<D.size;++rank)
        {
            p_domains.push_back( new domain<coord1D>(D,rank,ng,1,true) );
        }
        fflush(stderr);
        std::cerr << std::endl << std::endl;
        display<coord1D>(p_domains);

        std::cerr << "NOT Periodic:" << std::endl;
        std::cerr.flush();
        domain<coord1D>::list u_domains;
        for(size_t rank=0;rank<D.size;++rank)
        {
            u_domains.push_back(new domain<coord1D>(D,rank,ng,0,true) );
        }
        fflush(stderr);
        std::cerr << std::endl << std::endl;
        display<coord1D>(u_domains);

    }




    {

        const patch2D full( coord2D(1,1),coord2D(Nx,Ny) );
        std::cerr << std::endl;
        std::cerr << "#CPU=" << cpus << " in 2D: " << full << std::endl;
        coord2D       sizes;
        for(sizes.x=1;sizes.x<=cpus;++sizes.x)
        {
            for(sizes.y=1;sizes.y<=cpus;++sizes.y)
            {
                if(sizes.__prod()!=cpus) continue;
                std::cerr << "sizes=" << sizes << std::endl;
                divide::in2D D(sizes,full);
                {
                    std::cerr << "Periodic:" << std::endl;
                    std::cerr.flush();
                    coord2D pbcs(1,1);
                    domain<coord2D>::list domains;
                    for(size_t rank=0;rank<D.size;++rank)
                    {
                        domains.push_back( new domain<coord2D>(D,rank,ng,pbcs,true) );
                    }
                    fflush(stderr);
                    std::cerr << std::endl << std::endl;
                    display<coord2D>(domains);

                }
            }
            std::cerr << std::endl;
        }

    }

    return 0;


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

