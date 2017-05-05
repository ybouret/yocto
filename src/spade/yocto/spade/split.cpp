#include "yocto/spade/split.hpp"
#include "yocto/exception.hpp"

namespace yocto
{
    namespace spade
    {
        void split_ops:: basic(const size_t size,
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
                throw exception("spade.split: empty region fpr %u.%u", unsigned(size), unsigned(rank) );
            }
        }


        Layout1D split_ops::in1D(const size_t    size,
                                 const size_t    rank,
                                 const Layout1D &full)
        {
            coord1D offset = full.lower;
            coord1D length = full.width;
            basic(size,rank,offset,length);
            const coord1D lower = offset;
            const coord1D upper = offset+length-1;
            return Layout1D(lower,upper);
        }
    }
}
