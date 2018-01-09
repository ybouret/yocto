#include "yocto/ipso/domain.hpp"

namespace yocto
{
    namespace ipso
    {
        metrics:: ~metrics() throw() {}

        metrics:: metrics(const size_t num_items) :
        items( num_items ),
        async( 0 ),
        local( 0 )
        {
        }

        metrics:: metrics( const metrics &other ) :
        items( other.items ),
        async( other.async ),
        local( other.local )
        {
        }


        mpq metrics:: compute_alpha(const mpn &seqItems) const
        {
            assert(async>0);
            assert(items<=seqItems);
            const mpn num = seqItems - items;
            mpn       den = async;
            return mpq(num,den.inc());
        }

        mpq metrics:: compute_score(const mpq &alpha) const
        {
            const mpq todo(items);
            const mpq n_io(async);
            const mpq coms = alpha * n_io;
            return todo+coms;
        }


    }
}

