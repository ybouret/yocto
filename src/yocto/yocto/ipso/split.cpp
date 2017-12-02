#include "yocto/ipso/split.hpp"
#include "yocto/code/utils.hpp"

namespace yocto
{
    namespace ipso
    {

        coord1D split::in1D::computeCoresMap(const size_t cores, const coord1D length, const coord1D ) throw()
        {
            assert(length>0);
            if(cores<=1)
            {
                return 1;
            }
            else
            {
                return min_of<coord1D>(cores,length);
            }
        }


        static inline
        void getRunAndCom( coord1D &run, coord1D &com, const split::in2D &s ) throw()
        {
            for(size_t rank=0;rank<s.cores;++rank)
            {
                const patch2D sub = s(rank);
            }
        }

        coord2D split::in2D::computeCoresMap(const size_t  cores,
                                             const coord2D length,
                                             const coord2D pbc) throw()
        {
            if(cores<=1)
            {
                return coord2D(1,1);
            }

            const patch2D p(coord2D(1,1),coord2D(length));

            //__________________________________________________________________
            //
            // Run and Com for 1x1
            //__________________________________________________________________
            const   coord1D Lx   = length.x;
            const   coord1D Ly   = length.y;
            coord1D         Run1 = Lx*Ly;
            coord1D         Com1 = 0;
            if(pbc.x) Com1 += (Ly+Ly);
            if(pbc.y) Com1 += (Lx+Lx);

            //__________________________________________________________________
            //
            // Run and Com for 1x2
            //__________________________________________________________________
            split::in2D Split1x2(coord2D(1,2),p);
            coord1D  Run1x2 = 0;
            coord1D  Com1x2 = 0;
            getRunAndCom(Run1x2,Com1x2,Split1x2);

            return coord2D(1,1);
        }

    }

}


