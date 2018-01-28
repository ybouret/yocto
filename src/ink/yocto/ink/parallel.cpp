#include "yocto/ink/parallel.hpp"
#include <cstdlib>

namespace yocto
{
    namespace Ink
    {
        Splitter:: ~Splitter() throw() {}
        
        Splitter:: Splitter(const coord      user_sizes,
                            const Rectangle &rect) :
        Rectangle(rect),
        sizes(user_sizes),
        size( sizes.__prod() )
        {
            if(sizes.x<=0||sizes.y<=0) throw exception("Splitter: invalid sizes %dx%d", int(sizes.x), int(sizes.y) );
            assert(size>0);
            if(sizes.x>rect.w) throw exception("Splitter: sizes.x=%d>rect.w=%d",int(sizes.x), int(rect.w) );
            if(sizes.y>rect.h) throw exception("Splitter: sizes.y=%d>rect.h=%d",int(sizes.y), int(rect.y) );
        }
        
        
        coord Splitter:: getRanks(const size_t rank)  const throw()
        {
            assert(rank<size);
            const ldiv_t l = ldiv( long(rank), long(sizes.x) );
            assert(unit_t(l.rem) <sizes.x);
            assert(unit_t(l.quot)<sizes.y);
            return coord(l.rem,l.quot);
        }
        
        size_t Splitter:: getRank(const coord ranks) const throw()
        {
            assert(ranks.x>=0);
            assert(ranks.x<sizes.x);
            assert(ranks.y>=0);
            assert(ranks.y<sizes.y);
            
            return ranks.y * sizes.x + ranks.x;
        }
        
        Rectangle Splitter:: operator()(const size_t rank, coord *pRanks) const throw()
        {
            coord       offset(x,y);
            coord       length(w,h);
            const coord ranks = getRanks(rank);
            if(pRanks) *pRanks  = ranks;
            ipso::basic_split(ranks.x,sizes.x,offset.x,length.x);
            ipso::basic_split(ranks.y,sizes.y,offset.y,length.y);
            assert(length.x>0);
            assert(length.y>0);
            return Rectangle(offset.x,offset.y,length.x,length.y);
        }
        
    }
}

