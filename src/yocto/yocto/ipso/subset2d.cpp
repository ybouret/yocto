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

            std::cerr << "-- CROSS_SWAPS 2D from ranks=" << ranks << "@" << rank << std::endl;
            std::cerr << "-- flags=" << swaps::flg2str(flags) << std::endl;

            swaps_list Swaps;

            //------------------------------------------------------------------
            // going clockwise
            //------------------------------------------------------------------
            if( swaps::xup_yup == (flags & swaps::xup_yup) )
            {
                std::cerr << "\t@xup_yup=" << swaps::xup_yup << std::endl;
                const coord2D target_ranks = full.neighbor_ranks(ranks+coord2D(1,1));
                const size_t  target_rank  = full.get_rank_from(target_ranks);
                const coord2D inner_start(up.x-shift,up.y-shift);
                const coord2D outer_start(up.x+1,    up.y+1);
                swaps *swp = __create(*this,target_rank,swaps::xup_yup,inner_start,outer_start,width);
                Swaps.push_back(swp);
            }

            if( swaps::xup_ylo == (flags & swaps::xup_ylo) )
            {
                std::cerr << "\t@xup_ylo=" << swaps::xup_ylo << std::endl;
                const coord2D target_ranks = full.neighbor_ranks(ranks+coord2D(1,-1));
                const size_t  target_rank  = full.get_rank_from(target_ranks);
                const coord2D inner_start(up.x-shift,lo.y);
                const coord2D outer_start(up.x+1,    lo.y-width);
                swaps *swp = __create(*this,target_rank,swaps::xup_ylo,inner_start,outer_start,width);
                Swaps.push_back(swp);
            }

            if( swaps::xlo_ylo == (flags & swaps::xlo_ylo) )
            {
                std::cerr << "\t@xlo_ylo=" << swaps::xlo_ylo << std::endl;
                const coord2D target_ranks = full.neighbor_ranks(ranks+coord2D(-1,-1));
                const size_t  target_rank  = full.get_rank_from(target_ranks);
                const coord2D inner_start(lo.x,lo.y);
                const coord2D outer_start(lo.x-width,lo.y-width);
                swaps *swp = __create(*this,target_rank,swaps::xlo_ylo,inner_start,outer_start,width);
                Swaps.push_back(swp);
            }

            if( swaps::xlo_yup == (flags & swaps::xlo_yup) )
            {
                std::cerr << "\t@xlo_yup=" << swaps::xlo_yup << std::endl;
                const coord2D target_ranks = full.neighbor_ranks(ranks+coord2D(-1,-1));
                const size_t  target_rank  = full.get_rank_from(target_ranks);
                const coord2D inner_start(lo.x,up.y-shift);
                const coord2D outer_start(lo.x-width,up.y+1);
                swaps *swp = __create(*this,target_rank,swaps::xlo_yup,inner_start,outer_start,width);
                Swaps.push_back(swp);
            }
        }

    }
}


