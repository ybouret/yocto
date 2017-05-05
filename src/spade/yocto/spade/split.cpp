#include "yocto/spade/split.hpp"
#include "yocto/exception.hpp"

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

        Layout1D  Split:: In1D:: operator()(const size_t rank) const
        {
            coord1D offset = lower;
            coord1D length = width;
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
        x_size(max_of<size_t>(xcpu,1)),
        y_size(max_of<size_t>(ycpu,1)),
        size(x_size*y_size)
        {
            if(x_size>size_t(full.width.x)) throw exception("spade.split.in2d: too many X domains");
            if(y_size>size_t(full.width.y)) throw exception("spade.split.in2d: too many Y domains");
        }

        Split:: In2D:: ~In2D() throw()
        {
        }


        Layout2D Split::In2D:: operator()(size_t rank) const
        {
            assert(rank<size);
            const ldiv_t d         = ldiv(rank,x_size);
            const size_t y_rank    = d.quot;
            const size_t x_rank    = d.rem;
            coord2D      start     = lower;
            coord2D      final     = width;

            Basic(x_size, x_rank,start.x, final.x);
            Basic(y_size, y_rank,start.y, final.y);
            final += start;
            --final.x;
            --final.y;
            return Layout2D(start,final);
        }

    }

}

