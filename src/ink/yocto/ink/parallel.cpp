#include "yocto/ink/parallel.hpp"
#include "yocto/code/utils.hpp"
#include "yocto/sort/merge.hpp"
#include "yocto/ipso/mapping.hpp"
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
        jid(-1),
        cache()
        {
        }

        
    }

}



#include "yocto/ipso/subset.hpp"


namespace yocto
{
    namespace Ink
    {
        typedef ipso::patch2D                Patch;
        typedef ipso::mapping<ipso::coord2D> Mapping;
        typedef ipso::divide::in2D           Divider;

        Engine:: ~Engine() throw()
        {
        }

        Engine:: Engine(const Area         &area,
                        const SharedServer &sharedServer) :
        Area(area),
        sizes(),
        queue( sharedServer ),
        domains()
        {
            const Patch     p( coord(x,y), coord(x_end,y_end) );
            const size_t        n     = queue->num_threads();
            (coord &)sizes        = Mapping::optimal_sizes_for(n,p,0,ipso::coord2D(0,0),NULL);
            Divider         part(sizes,p);

            for(size_t rank=0;rank<part.size;++rank)
            {
                coord      ranks;
                const      Patch sub( part(rank,&ranks) );
                const Area rect( sub.lower.x, sub.lower.y, sub.width.x, sub.width.y );
                domains.push_back( new Domain(rect,ranks,rank) );
            }
        }

        size_t Engine:: size() const throw()
        {
            return domains.size;
        }

        const Domain * Engine::head() const throw() { return domains.head; }

        void Engine:: prepare(const size_t bytes_per_domain)
        {
            for(Domain *dom = domains.head; dom; dom=dom->next)
            {
                dom->cache.prepare(bytes_per_domain);
            }
        }

        void Engine:: flush() throw() { queue->flush(); }

    }

}



