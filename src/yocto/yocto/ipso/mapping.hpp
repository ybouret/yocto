#ifndef YOCTO_IPSO_MAPPING_INCLUDED
#define YOCTO_IPSO_MAPPING_INCLUDED 1

#include "yocto/ipso/subset.hpp"

namespace yocto
{
    namespace ipso
    {
        ////////////////////////////////////////////////////////////////////////
        //
        // automatic list of subsets
        //
        ////////////////////////////////////////////////////////////////////////
        template <typename COORD>
        class subsets : public object, public subset<COORD>::list
        {
        public:
            typedef core::list_of_cpp<subsets> list;
            
            
            const COORD   sizes;
            const score_t score;
            subsets      *next;
            subsets      *prev;
            
            inline virtual ~subsets() throw() {}
            
            inline explicit subsets(const divider<COORD> &full,
                                    const size_t          layers,
                                    const COORD           pbcs,
                                    const bool            build = false ) :
            subset<COORD>::list(),
            sizes( full.sizes ),
            score(),
            next(0),
            prev(0)
            {
                score_t &max_score = (score_t &)score;
                for(size_t rank=0;rank<full.size;++rank)
                {
                    subset<COORD> *sub = new subset<COORD>(full,rank,layers,pbcs,build);
                    this->push_back(sub);
                    const score_t &sub_score = sub->score;
                    if( __lexicographic_dec_fwd<size_t,3>(&sub_score,&max_score) < 0 )
                    {
                        max_score = sub_score;
                    }
                }
                assert(this->size==size_t(__coord_prod(sizes)));
            }
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(subsets);
        };
        
        ////////////////////////////////////////////////////////////////////////
        //
        // used to find optimial
        //
        ////////////////////////////////////////////////////////////////////////
        template <typename COORD>
        class mapping : public subsets<COORD>::list
        {
        public:
            static const size_t DIM = YOCTO_IPSO_DIM_OF(COORD);
            const subsets<COORD> * optimal;
            const subsets<COORD> * fallback;
            
            inline virtual ~mapping() throw() {}
            inline explicit mapping(const size_t        cpus,
                                    const patch<COORD> &full,
                                    const size_t        layers,
                                    const COORD         pbcs) :
            subsets<COORD>::list(),
            optimal(0),
            fallback(0)
            {
                setup(cpus,full,layers,pbcs);
                typename subsets<COORD>::list &self = *this;
                assert(self.size>0);
                core::merging< subsets<COORD> >::sort(self, compare_by_scores, NULL );
                optimal  = self.head; assert(optimal);
                fallback = self.tail; assert(fallback);
                for(const subsets<COORD> *subs=fallback;subs;subs=subs->prev)
                {
                    if(subs->size>=fallback->size)
                    {
                        fallback = subs;
                    }
                }
            }
            
            static inline
            COORD optimal_sizes_for(const size_t        cpus,
                                    const patch<COORD> &full,
                                    const size_t        layers,
                                    const COORD        &pbcs,
                                    COORD              *fallback)
            {
                const mapping maps(cpus,full,layers,pbcs);
                if(fallback)
                {
                    *fallback = maps.fallback->sizes;
                }
                return maps.optimal->sizes;
            }
            
            
        private:
            void setup(const size_t        cpus,
                       const patch<COORD> &full,
                       const size_t        layers,
                       const COORD         pbcs);
            
            static inline int compare_by_scores( const subsets<COORD> *lhs, const subsets<COORD> *rhs, void * ) throw()
            {
                const int ans = __lexicographic_inc_fwd<size_t,3>( &(lhs->score), &(rhs->score) );
                {
                    if(ans!=0)
                    {
                        // score is winning
                        return ans;
                    }
                    else
                    {
                        // same score : compare by increasing sum+coords
                        coord1D L[DIM+1] = { __coord_sum(lhs->sizes) };
                        coord1D R[DIM+1] = { __coord_sum(rhs->sizes) };
                        for(size_t i=0;i<DIM;++i)
                        {
                            L[i+1] = __coord(lhs->sizes,i);
                            R[i+1] = __coord(rhs->sizes,i);
                        }
                        return __lexicographic_inc_fwd<coord1D,DIM+1>(L,R);
                    }
                }
            }
            YOCTO_DISABLE_COPY_AND_ASSIGN(mapping);
        };
        
    }
}

#endif
