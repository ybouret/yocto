#include "yocto/spade/split.hpp"
#include "yocto/exception.hpp"

#include <cstdlib>

namespace yocto
{
    namespace spade
    {
        void Split:: Basic(const size_t size,
                               const size_t rank,
                               coord1D     &offset,
                               coord1D     &length)
        {
            assert(size>0);
            assert(rank<size);
            coord1D todo(length/size);
            for(size_t r=1;r<=rank;++r)
            {
                length -= todo;
                offset += todo;
                todo    = length/(size-r);
            }
            length = todo;
            if(length<=0)
            {
                throw exception("spade.split: empty region for %u.%u", unsigned(size), unsigned(rank) );
            }
        }


        Layout1D Split:: __in1D(const size_t    size,
                                const size_t    rank,
                                const Layout1D &full)
        {
            coord1D offset = full.lower;
            coord1D length = full.width;
            Basic(size,rank,offset,length);
            const coord1D lower = offset;
            const coord1D upper = offset+length-1;
            return Layout1D(lower,upper);
        }
    }
}

namespace yocto
{
    namespace spade
    {

        Split::In1D:: In1D(const Layout1D &full, const size_t ncpu):
        Layout1D(full),
        size(max_of<size_t>(1,ncpu))
        {
            if(size>size_t(full.width))
                throw exception("spade.split.in1d: too many domains");
        }

        Split:: In1D:: ~In1D() throw()
        {
        }

        Layout1D  Split:: In1D:: operator()(const coord1D rank) const
        {
            assert(rank>=0);
            assert(rank<coord1D(size));
            coord1D offset = lower;
            coord1D length = width;
            Basic(size,rank,offset,length);
            return Layout1D(offset,offset+length-1);
        }



    }
}

namespace yocto
{
    namespace spade
    {

        Split::In2D:: In2D(const Layout2D &full,
                           const size_t    xcpu,
                           const size_t    ycpu) :
        Layout2D(full),
        sizes(max_of<coord1D>(xcpu,1),max_of<coord1D>(ycpu,1)),
        size(sizes.x*sizes.y)
        {
            if(sizes.x>full.width.x) throw exception("spade.split.in2d: too many X domains");
            if(sizes.y>full.width.y) throw exception("spade.split.in2d: too many Y domains");
        }

        Split:: In2D:: ~In2D() throw()
        {
        }


        Layout2D Split::In2D:: operator()(const coord1D rank) const
        {
            assert(rank>=0);
            assert(rank<coord1D(size));
            const coord2D ranks  = getRanks(rank);
            coord2D       start  = lower;
            coord2D       final  = width;

            Basic(sizes.x, ranks.x,start.x, final.x);
            Basic(sizes.y, ranks.y,start.y, final.y);
            final += start;
            --final.x;
            --final.y;
            return Layout2D(start,final);
        }

        coord2D Split::In2D:: getRanks(const coord1D rank) const throw()
        {
            assert(rank>=0);
            assert(rank<coord1D(size));
            const ldiv_t  d         = ldiv(rank,sizes.x);
            const coord1D y_rank    = d.quot;
            const coord1D x_rank    = d.rem;

            return coord2D(x_rank,y_rank);
        }


    }

}


namespace yocto
{
    namespace spade
    {
        Split::In3D::In3D(const Layout3D &full,
                          const size_t    xcpu,
                          const size_t    ycpu,
                          const size_t    zcpu) :
        Layout3D(full),
        sizes(max_of<coord1D>(xcpu,1),max_of<coord1D>(ycpu,1),max_of<coord1D>(zcpu,1)),
        zcof(sizes.x*sizes.y),
        size(zcof*sizes.z)
        {
        }

        Split:: In3D:: ~In3D() throw()
        {
        }

        coord3D Split:: In3D:: getRanks(const coord1D rank) const throw()
        {
            assert(rank>=0);
            assert(rank<coord1D(size));
            const ldiv_t  dz     = ldiv(rank,zcof);
            const coord1D z_rank = dz.quot;
            const coord1D rz     = dz.rem;

            const ldiv_t  d      = ldiv(rz,sizes.x);
            const coord1D y_rank = d.quot;
            const coord1D x_rank = d.rem;

            return coord3D(x_rank,y_rank,z_rank);
        }

        Layout3D Split:: In3D:: operator()(const coord1D rank) const throw()
        {
            assert(rank>=0);
            assert(rank<coord1D(size));
            const coord3D ranks = getRanks(rank);
            coord3D       start  = lower;
            coord3D       final  = width;

            Basic(sizes.x, ranks.x,start.x, final.x);
            Basic(sizes.y, ranks.y,start.y, final.y);
            Basic(sizes.z, ranks.z,start.z, final.z);
            final += start;
            --final.x;
            --final.y;
            --final.z;
            return Layout3D(start,final);

        }


    }
}
