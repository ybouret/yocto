#include "yocto/spade/layout.hpp"
#include "yocto/exception.hpp"
#include "yocto/sort/network.hpp"

namespace yocto
{
    namespace spade
    {

        layout_info:: ~layout_info() throw()
        {
        }

        layout_info:: layout_info(const size_t dim):
        dimension(dim)
        {
            if(dimension<1||dimension>3)
                throw exception("space: layout dimension=%u", unsigned(dim));
        }

        layout_info:: layout_info(const layout_info &other) throw() :
        dimension(other.dimension)
        {
        }

        size_t layout_info:: setup(const void  *lower,
                                   const void  *upper,
                                   const void  *pitch,
                                   const void  *width) throw()
        {
            coord1D *Lower = (coord1D *)lower;
            coord1D *Upper = (coord1D *)upper;
            coord1D *Pitch = (coord1D *)pitch;
            coord1D *Width = (coord1D *)width;

            size_t nItems = 1;
            for(size_t i=0;i<dimension;++i)
            {
                netsort<coord1D>::level2(Lower[i],Upper[i]);
                nItems *= size_t(Width[i] = 1+Upper[i]-Lower[i]);
            }
            Pitch[0] = 1;
            for(size_t i=1;i<dimension;++i)
            {
                Pitch[i] = Pitch[i-1] * Width[i-1];
            }

            return nItems;
        }



    }

}
