#include "yocto/fame/mesh/rectilinear.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace fame;

template <typename T,typename COORD>
static inline void display_rmesh( const rectilinear_mesh<T,COORD> &mesh )
{
    std::cerr << "\t-- " << mesh.name << " --" << std::endl;
    for(size_t i=0;i<mesh.DIMENSION;++i)
    {
        std::cerr << "\t\t" << mesh[i].name << " inner=" << mesh[i].inner << std::endl;
        std::cerr << "\t\t" << mesh[i].name << " outer=" << mesh[i].outer << std::endl;

    }

}

YOCTO_UNIT_TEST_IMPL(mesh)
{

    {
        const layout<coord1d> full(1,16);
        {
            {
                std::cerr << "Full Periodic..." << std::endl;
                const domain<coord1d> dom0(0,1,NULL,full,1);
                {
                    std::cerr << "no ghost" << std::endl;
                    rectilinear_mesh<float,coord1d> rmesh("mesh1_full_per_no_ghost",dom0,0);
                    display_rmesh(rmesh);
                }
                {
                    std::cerr << "with ghost" << std::endl;
                    rectilinear_mesh<float,coord1d> rmesh("mesh1_full_not_w_ghosts",dom0,2);
                    display_rmesh(rmesh);
                }
            }

            {
                std::cerr << "Full Not Periodic..." << std::endl;
                const domain<coord1d> dom0(0,1,NULL,full,0);
                {
                    std::cerr << "no ghost" << std::endl;
                    rectilinear_mesh<float,coord1d> rmesh("mesh1",dom0,0);
                    display_rmesh(rmesh);
                }
                {
                    std::cerr << "with ghost" << std::endl;
                    rectilinear_mesh<float,coord1d> rmesh("mesh1",dom0,2);
                    display_rmesh(rmesh);
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
                        rectilinear_mesh<float,coord1d> rmesh("mesh1",dom0,1);
                        display_rmesh(rmesh);
                    }
                    {
                        std::cerr << "\t\tNOT PERIODIC" << std::endl;
                        const domain<coord1d> dom0(rank,size,NULL,full,0);
                        rectilinear_mesh<float,coord1d> rmesh("mesh1",dom0,1);
                        display_rmesh(rmesh);
                    }
                }
            }
        }
    }


    {
        std::cerr << std::endl << "2D" << std::endl;
        const layout<coord2d> full(coord2d(1,1), coord2d(16,16) );
        for(coord1d size=1;size<=4;++size)
        {
            std::cerr << std::endl << "size=" << size << std::endl;
            for(coord1d rank=0;rank<size;++rank)
            {
                std::cerr << "\trank=" << rank << std::endl;
                {
                    std::cerr << "\t\tPERIODIC" << std::endl;
                    const domain<coord2d> dom0(rank,size,NULL,full,coord2d(1,1));
                    rectilinear_mesh<float,coord2d> rmesh("mesh2",dom0,1);
                    display_rmesh(rmesh);
                }
                {
                    std::cerr << "\t\tNOT PERIODIC" << std::endl;
                    const domain<coord2d> dom0(rank,size,NULL,full,coord2d(0,0));
                    rectilinear_mesh<float,coord2d> rmesh("mesh2",dom0,1);
                    display_rmesh(rmesh);
                }

            }
        }
    }

    {
        std::cerr << std::endl << "3D" << std::endl;
        const layout<coord3d> full(coord3d(1,1,1), coord3d(16,16,16) );
        for(coord1d size=1;size<=4;++size)
        {
            std::cerr << std::endl << "size=" << size << std::endl;
            for(coord1d rank=0;rank<size;++rank)
            {
                std::cerr << "\trank=" << rank << std::endl;
                {
                    std::cerr << "\t\tPERIODIC" << std::endl;
                    const domain<coord3d> dom0(rank,size,NULL,full,coord3d(1,1,1));
                    rectilinear_mesh<double,coord3d> rmesh("mesh3",dom0,1);
                    display_rmesh(rmesh);
                }
                {
                    std::cerr << "\t\tNOT PERIODIC" << std::endl;
                    const domain<coord3d> dom0(rank,size,NULL,full,coord3d(0,0,0));
                    rectilinear_mesh<double,coord3d> rmesh("mesh3",dom0,1);
                    display_rmesh(rmesh);
                }

            }
        }
    }



}
YOCTO_UNIT_TEST_DONE()
