#include "yocto/ipso/subset.hpp"
#include "yocto/ptr/auto.hpp"

namespace yocto
{
    namespace ipso
    {

        static inline
        swaps *__create(const subset<coord2D> &sub,
                        const size_t   target_rank,
                        const unsigned where,
                        const coord2D  inner_start,
                        const coord2D  outer_start,
                        const coord1D  width)
        {
            assert(width>0);
            auto_ptr<swaps> swp( new swaps(sub.rank,target_rank,width,where) );
            (size_t &)(swp->count) = width*width;
            swp->allocate();
            swap & _recv = (swap &)(swp->recv);
            swap & _send = (swap &)(swp->send);

            size_t q = 0;
            for(coord1D dy=0,iy=inner_start.y,oy=outer_start.y;dy<width;++dy,++iy,++oy)
            {
                for(coord1D dx=0,ix=inner_start.x,ox=outer_start.x;dx<width;++dx,++ix,++ox)
                {
                    const coord2D send_coord(ix,iy);
                    const coord2D recv_coord(ox,oy);
                    assert(sub.outer.has(send_coord));
                    assert(sub.outer.has(recv_coord));
                    const coord1D idx_send = sub.outer.offset_of(send_coord);
                    const coord1D idx_recv = sub.outer.offset_of(recv_coord);
                    ++q;
                    _send[q] = idx_send;
                    _recv[q] = idx_recv;
                }
            }
            swp->io_check();
            return swp.yield();
        }


        inline static bool __matches( const unsigned mask, const unsigned flags ) throw()
        {
            return mask == ( flags & mask );
        }

        template <>
        void subset<coord2D>::load_cross_swaps(const divider<coord2D> &full,
                                               const size_t            layers,
                                               const coord2D           pbcs,
                                               const bool              build)
        {
            assert(layers>0);
            //------------------------------------------------------------------
            // gather information
            //------------------------------------------------------------------
            const coord1D width    = layers;
            const coord1D shift    = width-1;
            const coord2D up       = inner.upper;
            const coord2D lo       = inner.lower;
            

#define Y_SUB2D_APEX(FLAGS) \
do {  push_back_apex( __create(*this,full.get_rank_from(target_ranks),FLAGS,inner_start,outer_start,width) ); } while(false)

            //------------------------------------------------------------------
            // going clockwise
            //------------------------------------------------------------------
            if( __matches(swaps::xup_yup,flags) )
            {
                //std::cerr << "\t@xup_yup=" << swaps::xup_yup << std::endl;
                const coord2D target_ranks = full.neighbor_ranks(ranks+coord2D(1,1));
                const coord2D inner_start(up.x-shift,up.y-shift);
                const coord2D outer_start(up.x+1,    up.y+1);
                Y_SUB2D_APEX(swaps::xup_yup);
            }

            if( __matches(swaps::xup_ylo,flags) )
            {
                //std::cerr << "\t@xup_ylo=" << swaps::xup_ylo << std::endl;
                const coord2D target_ranks = full.neighbor_ranks(ranks+coord2D(1,-1));
                const coord2D inner_start(up.x-shift,lo.y);
                const coord2D outer_start(up.x+1,    lo.y-width);
                Y_SUB2D_APEX(swaps::xup_ylo);
            }

            if( __matches(swaps::xlo_ylo,flags) )
            {
                //std::cerr << "\t@xlo_ylo=" << swaps::xlo_ylo << std::endl;
                const coord2D target_ranks = full.neighbor_ranks(ranks+coord2D(-1,-1));
                const coord2D inner_start(lo.x,lo.y);
                const coord2D outer_start(lo.x-width,lo.y-width);
                Y_SUB2D_APEX(swaps::xlo_ylo);
            }

            if( __matches(swaps::xlo_yup,flags) )
            {
                //std::cerr << "\t@xlo_yup=" << swaps::xlo_yup << std::endl;
                const coord2D target_ranks = full.neighbor_ranks(ranks+coord2D(-1,1));
                const coord2D inner_start(lo.x,up.y-shift);
                const coord2D outer_start(lo.x-width,up.y+1);
                Y_SUB2D_APEX(swaps::xlo_yup);
            }

            for(size_t diag=0;diag<2;++diag)
            {
                apex_local[diag].__sort();
                apex_async[diag].__sort();
            }
        }

    }
}


