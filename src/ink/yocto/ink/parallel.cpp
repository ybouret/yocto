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

#if 0
namespace yocto
{
    namespace Ink
    {

        Partition:: ~Partition() throw() {}
        
        Partition:: Partition(const coord      user_sizes,
                              const Area      &rect) throw():
        Area(rect),
        sizes(user_sizes),
        cores( sizes.__prod() ),
        score(0),
        next(0),
        prev(0)
        {
            //! check sanity
            assert(sizes.x>0);
            assert(sizes.y>0);
            assert(cores>0);
            assert(sizes.x<=w);
            assert(sizes.y<=h);
        }

        void Partition:: compute( Domain::List *domains ) const
        {
            size_t    &smax = (size_t &)score;
            smax = 0;
            if(domains) domains->clear();
            for(size_t rank = 0;rank<cores;++rank)
            {
                coord           ranks;
                const Area      sub   = (*this)(rank,&ranks);
                size_t          count = 0;
                if(domains)
                {
                    Domain *dom = new Domain(sub,ranks,rank);
                    domains->push_back(dom);
                    count = dom->items;
                }
                else
                {
                    count = sub.w*sub.h;
                }
                if(count>smax) smax=count;
            }
            //std::cerr << "sizes=" << sizes << " => #core=" << cores << " : score=" << score << std::endl;
        }
        
        coord Partition:: getRanks(const size_t rank)  const throw()
        {
            assert(rank<cores);
            const ldiv_t l = ldiv( long(rank), long(sizes.x) );
            assert(unit_t(l.rem) <sizes.x);
            assert(unit_t(l.quot)<sizes.y);
            return coord(l.rem,l.quot);
        }
        
        size_t Partition:: getRank(const coord ranks) const throw()
        {
            assert(ranks.x>=0);
            assert(ranks.x<sizes.x);
            assert(ranks.y>=0);
            assert(ranks.y<sizes.y);
            
            return ranks.y * sizes.x + ranks.x;
        }
        
        Area  Partition:: operator()(const size_t rank, coord *pRanks) const throw()
        {
            unit_t X = x; assert(0==X);
            unit_t Y = y; assert(0==Y);
            unit_t W = w;
            unit_t H = h;
            const coord ranks   = getRanks(rank);
            if(pRanks) *pRanks  = ranks;
            ipso::basic_split(ranks.x,sizes.x,X,W); assert(W>0);
            ipso::basic_split(ranks.y,sizes.y,Y,H); assert(H>0);
            return Area(X,Y,W,H);
        }

        static inline
        int __compare_sizes(const coord &l, const coord &r) throw()
        {
            assert(l.x>0); assert(l.y>0);
            assert(r.x>0); assert(r.y>0);
            const unit_t l_norm = l.__sum();
            const unit_t r_norm = r.__sum();
            if(l_norm<r_norm)
            {
                return -1;
            }
            else
            {
                if(r_norm<l_norm)
                {
                    return 1;
                }
                else
                {
                    return __lexicographic_inc_fwd<unit_t,2>(&l,&r);
                }
            }
        }

        static inline
        int __compare_partitions( const Partition *lhs, const Partition *rhs, void *) throw()
        {
            const size_t L = lhs->score;
            const size_t R = rhs->score;
            if(L<R)
            {
                return -1;
            }
            else
            {
                if(R<L)
                {
                    return 1;
                }
                else
                {
                    // same score
                    return __compare_sizes(lhs->sizes,rhs->sizes);
                }
            }
        }

        void Partition:: Build(List            &parts,
                               const Area      &full,
                               const size_t     max_cpus )
        {
            const unit_t n         = max_of<size_t>(1,max_cpus);
            const unit_t nx        = min_of<unit_t>(n,full.w);
            const unit_t ny        = min_of<unit_t>(n,full.h);
            coord        sizes;

            for(sizes.x=1;sizes.x<=nx;++sizes.x)
            {
                for(sizes.y=1;sizes.y<=ny;++sizes.y)
                {
                    const unit_t nn = sizes.__prod();
                    if(nn>n) continue;
                    parts.push_back( new Partition(sizes,full) );
                    parts.tail->compute(NULL);
                }
            }

            core::merging<Partition>::sort(parts,__compare_partitions,NULL);
            
        }

        coord  Partition:: Optimal(const Area &full, const size_t max_cpus)
        {
            List parts;
            Build(parts,full,max_cpus);
            if(parts.size)
            {
                return parts.head->sizes;
            }
            else
            {
                return coord(0,0);
            }
        }


    }
}
#endif

#include "yocto/ipso/divide.hpp"

namespace yocto
{
    namespace Ink
    {
        typedef ipso::divide::in2D Divider;

        Domains:: ~Domains() throw()
        {
        }

        coord Domains:: GetPartitionFor(const Area &area, const SharedServer &srv) throw()
        {
            const Patch  p = area.getPatch();
            const size_t n = srv->cpu.num_threads();
            return Divider::optimal_for(p,n,NULL);
        }

        void Domains:: setup(const Area &full)
        {
            const Patch     p     = full.getPatch();
            const size_t    n     = engine->cpu.num_threads();
            (coord &)sizes        = Divider::optimal_for(p,n,NULL);
            Divider         part(sizes,p);
            
            for(size_t rank=0;rank<part.size;++rank)
            {
                coord      ranks;
                const Area rect( part(rank,&ranks) );
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

