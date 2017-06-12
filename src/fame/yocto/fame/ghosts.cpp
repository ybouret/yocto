#include "yocto/fame/layouts.hpp"


namespace yocto
{
    namespace fame
    {
        ghosts_pair:: ~ghosts_pair() throw()
        {
        }


        ghosts_pair:: ghosts_pair(const coord1d r, const size_t n) :
        ghosts_type(),
        rank(r),
        outer( next_array() ),
        inner( next_array() )
        {
            allocate(n);
        }
        
    }
}

namespace yocto
{
    namespace fame
    {

        ghosts:: ghosts() throw() : prev(0), next(0)
        {
        }
        
        ghosts:: ~ghosts() throw()
        {
            cleanup();
        }

        void ghosts:: cleanup() throw()
        {
            if(next) { delete next; next = 0; }
            if(prev) { delete prev; prev = 0; }
        }
    }
}


namespace yocto
{
    namespace fame
    {
        template <>
        ghosts_of<coord1d>:: ghosts_of( const layouts<coord1d> &L )
        {
            ghosts_base            &the_ghosts = *this;
            const size_t            num_ghosts = L.depth;
            const size_t            ng         = num_ghosts * 1;
            const layout<coord1d>  &inner      = L.inner;
            const layout<coord1d>  &outer      = L.outer;

            const domain<coord1d>::link &xlnk = L.links[0];
            ghosts                      &G    = the_ghosts[0];
            if(xlnk.prev)
            {
                coord1d o = outer.lower;
                coord1d i = inner.lower;
                ghosts_pair &gp = * (G.prev=new ghosts_pair(xlnk.prev->rank,ng));
                for(size_t g=1;g<=num_ghosts;++g,++o,++i)
                {
                    gp.inner[g] = outer.offset_of(i);
                    gp.outer[g] = outer.offset_of(o);
                }
            }

            if(xlnk.next)
            {
                coord1d o = outer.upper-num_ghosts;
                coord1d i = inner.upper-num_ghosts;
                ghosts_pair &gp = * (G.next=new ghosts_pair(xlnk.next->rank,ng));
                for(size_t g=1;g<=num_ghosts;++g,++o,++i)
                {
                    gp.inner[g] = outer.offset_of(i);
                    gp.outer[g] = outer.offset_of(o);
                }

            }


        }
    }
}
