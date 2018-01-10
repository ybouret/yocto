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
            assert(items<=seqItems);
            const mpn num = seqItems - items;
            mpn       den = async;
            return mpq(num,den.inc());
        }
        
        void metrics:: compute_copy_rates( copy_rates &rates, const metrics &seq ) const
        {
            assert(items<=seq.items);
            assert(local<=seq.local);
            const mpn a_num = seq.items - items;
            const mpn l_num = seq.local - local;
            mpn       den   = async;
            (void)den.inc();
            rates.async = mpq(a_num,den);
            rates.local = mpq(l_num,den);
        }


        mpq metrics:: compute_score(const mpq &alpha) const
        {
            const mpq todo(items);
            const mpq n_io(async);
            const mpq coms = alpha * n_io;
            return todo+coms;
        }

        void metrics:: compute_scores( scores &s, const copy_rates &rates) const
        {
            const mpq todo(items);
            const mpq n_io(async);
            const mpq coms = rates.async * n_io;
            s.wxch = todo + coms;

            const mpq n_cp(local);
            s.copy = n_cp;
            s.copy += rates.local * todo;

        }

    }
}

