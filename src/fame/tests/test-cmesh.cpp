#include "yocto/fame/mesh/curvilinear.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace fame;

template <typename T,typename COORD>
static inline void display_cmesh(const curvilinear_mesh<T,COORD> &cmesh )
{
    const size_t DIMENSION = YOCTO_FAME_DIM_OF(COORD);
    for(size_t dim=0;dim<DIMENSION;++dim)
    {
        const typename curvilinear_mesh<T,COORD>::coords_type &coords = cmesh[dim];
        std::cerr << "coords#0 inner=" << coords.inner << std::endl;
        std::cerr << "coords#0 outer=" << coords.outer << std::endl;
    }
    std::cerr << std::endl;

}

YOCTO_UNIT_TEST_IMPL(cmesh)
{

    {
        const layout<coord1d> full(1,32);

        {
            std::cerr << "Full Periodic..." << std::endl;
            const domain<coord1d> dom0(0,1,NULL,full,1);
            {
                std::cerr << "no ghost" << std::endl;
                curvilinear_mesh<float,coord1d> cmesh("mesh1_full_per_no_ghost",dom0,0);
                display_cmesh(cmesh);
            }
            {
                std::cerr << "with ghost" << std::endl;
                curvilinear_mesh<float,coord1d> cmesh("mesh1_full_not_w_ghosts",dom0,2);
                display_cmesh(cmesh);
            }

        }


        {
            std::cerr << "Full Not Periodic..." << std::endl;
            const domain<coord1d> dom0(0,1,NULL,full,0);
            {
                std::cerr << "no ghost" << std::endl;
                curvilinear_mesh<float,coord1d> cmesh("mesh1",dom0,0);
                display_cmesh(cmesh);
            }
            {
                std::cerr << "with ghost" << std::endl;
                curvilinear_mesh<float,coord1d> cmesh("mesh1",dom0,2);
                display_cmesh(cmesh);
            }

        }

        for(coord1d size=1;size<=4;++size)
        {
            std::cerr << std::endl << "size=" << size << std::endl;
            for(coord1d rank=0;rank<size;++rank)
            {
                std::cerr << "\trank=" << rank << std::endl;
                {
                    std::cerr << "\t\tPERIODIC" << std::endl;
                    const domain<coord1d> dom0(rank,size,NULL,full,1);
                    curvilinear_mesh<float,coord1d> cmesh("mesh1",dom0,1);
                    display_cmesh(cmesh);
                }
                {
                    std::cerr << "\t\tNOT PERIODIC" << std::endl;
                    const domain<coord1d> dom0(rank,size,NULL,full,0);
                    curvilinear_mesh<float,coord1d> cmesh("mesh1",dom0,1);
                    display_cmesh(cmesh);
                }
            }
        }



    }
}
YOCTO_UNIT_TEST_DONE()
