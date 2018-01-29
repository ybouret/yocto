#include "yocto/ink/parallel.hpp"
#include "yocto/code/utils.hpp"
#include "yocto/sort/merge.hpp"
#include <cstdlib>

namespace yocto
{
    namespace Ink
    {

        Domain:: ~Domain() throw() {}
        Domain:: Domain(const Rectangle &user_rect,
                        const coord     &user_ranks) :
        Rectangle(user_rect),
        ranks(user_ranks),
        items(w*h),
        next(0),
        prev(0)
        {
        }

        

        Partition:: ~Partition() throw() {}
        
        Partition:: Partition(const coord      user_sizes,
                              const Rectangle &rect) throw():
        Rectangle(rect),
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

        void Partition:: computeScore( Domain::List *domains ) const
        {
            size_t    &smax = (size_t &)score;
            smax = 0;
            for(size_t rank = 0;rank<cores;++rank)
            {
                coord           ranks;
                const Rectangle sub   = (*this)(rank,&ranks);
                size_t          count = 0;
                if(domains)
                {
                    Domain *dom = new Domain(sub,ranks);
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
        
        Rectangle Partition:: operator()(const size_t rank, coord *pRanks) const throw()
        {
            unit_t X = x; assert(0==X);
            unit_t Y = y; assert(0==Y);
            unit_t W = w;
            unit_t H = h;
            const coord ranks   = getRanks(rank);
            if(pRanks) *pRanks  = ranks;
            ipso::basic_split(ranks.x,sizes.x,X,W); assert(W>0);
            ipso::basic_split(ranks.y,sizes.y,Y,H); assert(H>0);
            return Rectangle(X,Y,W,H);
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
                               const Rectangle &full,
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
                    parts.tail->computeScore(NULL);
                }
            }

            core::merging<Partition>::sort(parts,__compare_partitions,NULL);
            
        }

        coord  Partition:: Optimal(const Rectangle &full, const size_t max_cpus)
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

