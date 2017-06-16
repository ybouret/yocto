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

        ghosts:: ghosts() throw() :
        prev(0),
        next(0),
        kind(empty),
        flag(has_none)
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

        const char  * ghosts:: kind_text() const throw()
        {
            switch(kind)
            {
                case empty: return "EMPTY";
                case async: return "ASYNC";
                case lcopy: return "LCOPY";
            }
            return "UNKNOWN";
        }

        size_t ghosts:: size() const throw()
        {
            if(prev)
            {
                assert(!(next!=0&&next->size!=prev->size));
                return prev->size;
            }
            else
            {
                if(next)
                {
                    assert(0==prev);
                    return next->size;
                }
                else
                {
                    return 0;
                }
            }
        }

    }
}


namespace yocto
{
    namespace fame
    {

        ////////////////////////////////////////////////////////////////////////
        //
        // 1 D ghost
        //
        ////////////////////////////////////////////////////////////////////////

        template <>
        ghosts_of<coord1d>:: ghosts_of( const layouts<coord1d> &L ) :
        ghosts_base<coord1d>(L.self.rank)
        {
            ghosts_base<coord1d>   &the_ghosts = *this;
            const size_t            num_ghosts = L.depth;
            const size_t            num_shifts = num_ghosts-1;
            const size_t            ng         = num_ghosts * 1;
            const layout<coord1d>  &inner      = L.inner;
            const layout<coord1d>  &outer      = L.outer;

            const domain<coord1d>::link &xlnk = L.links[0];
            ghosts                      &G    = the_ghosts[0];

#define Y_FAME_G1D()                      \
do {                                      \
for(size_t g=1;g<=num_ghosts;++g,++o,++i) \
{                                         \
gp.inner[g] = outer.offset_of(i);         \
gp.outer[g] = outer.offset_of(o);         \
}                                         \
}                                         \
while(false)

            if(xlnk.prev)
            {
                coord1d o = outer.lower;
                coord1d i = inner.lower;
                ghosts_pair &gp = * (G.prev=new ghosts_pair(xlnk.prev->rank,ng));
                Y_FAME_G1D();
            }

            if(xlnk.next)
            {
                coord1d i = inner.upper-num_shifts;
                coord1d o = outer.upper-num_shifts;
                ghosts_pair &gp = * (G.next=new ghosts_pair(xlnk.next->rank,ng));
                Y_FAME_G1D();
            }

            // finalize
            collect_exchange_info();
        }
    }
}


namespace yocto
{
    namespace fame
    {
        template <>
        ghosts_of<coord2d>:: ghosts_of( const layouts<coord2d> &L ) :
        ghosts_base<coord2d>(L.self.rank)
        {
            ghosts_base<coord2d>   &the_ghosts = *this;
            const size_t            num_ghosts = L.depth;
            const size_t            num_shifts = num_ghosts-1;
            const layout<coord2d>  &inner      = L.inner;
            const layout<coord2d>  &outer      = L.outer;

            //__________________________________________________________________
            //
            // along X
            //__________________________________________________________________
#define Y_FAME_G2D_X() do {                       \
size_t       gg = 0;                              \
for(size_t g=1;g<=num_ghosts;++g,++o,++i)         \
{                                                 \
for(coord1d y=outer.lower.y;y<=outer.upper.y;++y) \
{                                                 \
const coord2d I(i,y);                             \
const coord2d O(o,y);                             \
++gg;                                             \
gp.inner[gg] = outer.offset_of(I);                \
gp.outer[gg] = outer.offset_of(O);                \
}                                                 \
}                                                 \
assert(ng==gg);                                   \
} while(false)

            {
                const domain<coord2d>::link &lnk   = L.links[0];
                ghosts                      &G     = the_ghosts[0];
                const size_t                 ng    = num_ghosts * outer.width.y;
                if(lnk.prev)
                {
                    coord1d      i  = inner.lower.x;
                    coord1d      o  = outer.lower.x;
                    ghosts_pair &gp = * (G.prev= new ghosts_pair(lnk.prev->rank,ng) );
                    Y_FAME_G2D_X();
                }

                if(lnk.next)
                {
                    coord1d      i  = inner.upper.x-num_shifts;
                    coord1d      o  = outer.upper.x-num_shifts;
                    ghosts_pair &gp = * (G.next= new ghosts_pair(lnk.next->rank,ng) );
                    Y_FAME_G2D_X();
                }

            }


            //__________________________________________________________________
            //
            // along Y
            //__________________________________________________________________
#define Y_FAME_G2D_Y()                            \
do {                                              \
size_t gg = 0;                                    \
for(size_t g=1;g<=num_ghosts;++g,++o,++i)         \
{                                                 \
for(coord1d x=outer.lower.x;x<=outer.upper.x;++x) \
{                                                 \
const coord2d I(x,i);                             \
const coord2d O(x,o);                             \
++gg;                                             \
gp.inner[gg] = outer.offset_of(I);                \
gp.outer[gg] = outer.offset_of(O);                \
}                                                 \
}                                                 \
assert(ng==gg);                                   \
} while(false)

            {
                const domain<coord2d>::link &lnk   = L.links[1];
                ghosts                      &G     = the_ghosts[1];
                const size_t                 ng    = num_ghosts * outer.width.x;

                if(lnk.prev)
                {
                    coord1d      i  = inner.lower.y;
                    coord1d      o  = outer.lower.y;
                    ghosts_pair &gp = * (G.prev= new ghosts_pair(lnk.prev->rank,ng) );
                    Y_FAME_G2D_Y();
                }

                if(lnk.next)
                {
                    coord1d      i  = inner.upper.y-num_shifts;
                    coord1d      o  = outer.upper.y-num_shifts;
                    ghosts_pair &gp = * (G.next= new ghosts_pair(lnk.next->rank,ng) );
                    Y_FAME_G2D_Y();
                }

            }

            collect_exchange_info();
        }

    }

}

namespace yocto
{
    namespace fame
    {
        template <>
        ghosts_of<coord3d>:: ghosts_of( const layouts<coord3d> &L ) :
        ghosts_base<coord3d>(L.self.rank)
        {
            ghosts_base<coord3d>   &the_ghosts = *this;
            const size_t            num_ghosts = L.depth;
            const size_t            num_shifts = num_ghosts-1;
            const layout<coord3d>  &inner      = L.inner;
            const layout<coord3d>  &outer      = L.outer;

            //__________________________________________________________________
            //
            // along X
            //__________________________________________________________________
            {
                const domain<coord3d>::link &lnk   = L.links[0];
                ghosts                      &G     = the_ghosts[0];
                const size_t                 ng    = num_ghosts * outer.width.y * outer.width.z;
                if(lnk.prev)
                {
                    ghosts_pair &gp = * (G.prev= new ghosts_pair(lnk.prev->rank,ng) );
                    size_t       gg = 0;
                    for(coord1d z=outer.lower.z;z<=outer.upper.z;++z)
                    {
                        for(coord1d y=outer.lower.y;y<=outer.upper.y;++y)
                        {
                            coord1d      i  = inner.lower.x;
                            coord1d      o  = outer.lower.x;
                            for(size_t g=1;g<=num_ghosts;++g,++o,++i)
                            {

                                const coord3d I(i,y,z);
                                const coord3d O(o,y,z);
                                ++gg;
                                gp.inner[gg] = outer.offset_of(I);
                                gp.outer[gg] = outer.offset_of(O);
                            }
                        }
                    }
                    assert(ng==gg);
                }

                if(lnk.next)
                {
                    ghosts_pair &gp = * (G.next = new ghosts_pair(lnk.next->rank,ng) );
                    size_t       gg = 0;
                    for(coord1d z=outer.lower.z;z<=outer.upper.z;++z)
                    {
                        for(coord1d y=outer.lower.y;y<=outer.upper.y;++y)
                        {
                            coord1d      i  = inner.upper.x-num_shifts;
                            coord1d      o  = outer.upper.x-num_shifts;
                            for(size_t g=1;g<=num_ghosts;++g,++o,++i)
                            {

                                const coord3d I(i,y,z);
                                const coord3d O(o,y,z);
                                ++gg;
                                gp.inner[gg] = outer.offset_of(I);
                                gp.outer[gg] = outer.offset_of(O);
                            }
                        }
                    }
                    assert(ng==gg);
                }

            }


            //__________________________________________________________________
            //
            // along Y
            //__________________________________________________________________
            {
                const domain<coord3d>::link &lnk   = L.links[1];
                ghosts                      &G     = the_ghosts[1];
                const size_t                 ng    = num_ghosts * outer.width.x * outer.width.z;
                if(lnk.prev)
                {
                    ghosts_pair &gp = * (G.prev= new ghosts_pair(lnk.prev->rank,ng) );
                    size_t       gg = 0;
                    for(coord1d z=outer.lower.z;z<=outer.upper.z;++z)
                    {
                        coord1d      i  = inner.lower.y;
                        coord1d      o  = outer.lower.y;
                        for(size_t g=1;g<=num_ghosts;++g,++o,++i)
                        {
                            for(coord1d x=outer.lower.x;x<=outer.upper.x;++x)
                            {
                                const coord3d I(x,i,z);
                                const coord3d O(x,o,z);
                                ++gg;
                                gp.inner[gg] = outer.offset_of(I);
                                gp.outer[gg] = outer.offset_of(O);
                            }
                        }
                    }
                    assert(ng==gg);
                }

                if(lnk.next)
                {
                    ghosts_pair &gp = * (G.next = new ghosts_pair(lnk.next->rank,ng) );
                    size_t       gg = 0;
                    for(coord1d z=outer.lower.z;z<=outer.upper.z;++z)
                    {
                        coord1d      i  = inner.upper.y-num_shifts;
                        coord1d      o  = outer.upper.y-num_shifts;
                        for(size_t g=1;g<=num_ghosts;++g,++o,++i)
                        {
                            for(coord1d x=outer.lower.x;x<=outer.upper.x;++x)
                            {
                                const coord3d I(x,i,z);
                                const coord3d O(x,o,z);
                                ++gg;
                                gp.inner[gg] = outer.offset_of(I);
                                gp.outer[gg] = outer.offset_of(O);
                            }
                        }
                    }
                    assert(ng==gg);
                }

            }


            //__________________________________________________________________
            //
            // along Z
            //__________________________________________________________________
            {
                const domain<coord3d>::link &lnk   = L.links[2];
                ghosts                      &G     = the_ghosts[2];
                const size_t                 ng    = num_ghosts * outer.width.x * outer.width.y;
                if(lnk.prev)
                {
                    ghosts_pair &gp = * (G.prev= new ghosts_pair(lnk.prev->rank,ng) );
                    size_t       gg = 0;
                    coord1d      i  = inner.lower.z;
                    coord1d      o  = outer.lower.z;
                    for(size_t g=1;g<=num_ghosts;++g,++o,++i)
                    {
                        for(coord1d y=outer.lower.y;y<=outer.upper.y;++y)
                        {
                            for(coord1d x=outer.lower.x;x<=outer.upper.x;++x)
                            {
                                const coord3d I(x,y,i);
                                const coord3d O(x,y,o);
                                ++gg;
                                gp.inner[gg] = outer.offset_of(I);
                                gp.outer[gg] = outer.offset_of(O);
                            }
                        }
                    }
                }

                if(lnk.next)
                {
                    ghosts_pair &gp = * (G.next = new ghosts_pair(lnk.next->rank,ng) );
                    size_t       gg = 0;
                    coord1d      i  = inner.upper.z-num_shifts;
                    coord1d      o  = outer.upper.z-num_shifts;
                    for(size_t g=1;g<=num_ghosts;++g,++o,++i)
                    {
                        for(coord1d y=outer.lower.y;y<=outer.upper.y;++y)
                        {
                            for(coord1d x=outer.lower.x;x<=outer.upper.x;++x)
                            {
                                const coord3d I(x,y,i);
                                const coord3d O(x,y,o);
                                ++gg;
                                gp.inner[gg] = outer.offset_of(I);
                                gp.outer[gg] = outer.offset_of(O);
                            }
                        }
                    }
                }


            }

            collect_exchange_info();
        }
        
    }
}




