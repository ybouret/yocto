#include "yocto/ink/parallel.hpp"
#include "yocto/code/utils.hpp"
#include "yocto/sort/merge.hpp"
#include <cstdlib>

namespace yocto
{
    namespace Ink
    {

        Domain:: ~Domain() throw() {}
        Domain:: Domain(const Area      &user_rect,
                        const coord     &user_ranks,
                        const size_t     user_rank) throw():
        Area(user_rect),
        ranks(user_ranks),
        items(w*h),
        rank(user_rank),
        next(0),
        prev(0),
        jid(-1)
        {
        }

        
    }

}



#include "yocto/ipso/divide.hpp"

namespace yocto
{
    namespace Ink
    {
        typedef ipso::patch2D      Patch;
        typedef ipso::divide::in2D Divider;

        Domains:: ~Domains() throw()
        {
        }

        coord Domains:: GetPartitionFor(const Area &area, const SharedServer &srv) throw()
        {

            const Patch  p( coord(area.x,area.y), coord(area.x_end,area.y_end) );
            const size_t n = srv->cpu.num_threads();
            return Divider::optimal_for(p,n,NULL);
        }

        void Domains:: setup(const Area &area)
        {
            const Patch     p( coord(area.x,area.y), coord(area.x_end,area.y_end) );
            const size_t    n     = engine->cpu.num_threads();
            (coord &)sizes        = Divider::optimal_for(p,n,NULL);
            Divider         part(sizes,p);
            
            for(size_t rank=0;rank<part.size;++rank)
            {
                coord      ranks;
                const      Patch sub( part(rank,&ranks) );
                const Area rect( sub.lower.x, sub.lower.y, sub.width.x, sub.width.y );
                this->push_back( new Domain(rect,ranks,rank) );
            }
        }



        Domains:: Domains(const Area &full, const SharedServer &user_engine) :
        Domain::List(),
        engine(user_engine)
        {
            setup(full);
        }

        Domains:: Domains(const Area &full, ThreadServer *user_engine) :
        Domain::List(),
        engine(user_engine)
        {
            setup(full);
        }
    }

}

