#ifndef YOCTO_IPSO_METRICS_INCLUDED
#define YOCTO_IPSO_METRICS_INCLUDED 1

#include "yocto/mpl/rational.hpp"
#include "yocto/container/tuple.hpp"
#include <cstdio>

namespace yocto
{
    namespace ipso
    {
        ////////////////////////////////////////////////////////////////////////
        //
        //! a domain's metrics in terms or items to compute, async and local copy
        //
        ////////////////////////////////////////////////////////////////////////

        class metrics
        {
        public:
            typedef mpq type;

            const type items;
            const type async;
            const type local;

            explicit metrics(const size_t num_items);
            virtual ~metrics() throw();
            metrics(const metrics&);
            friend inline std::ostream & operator<<( std::ostream &os, const metrics &m )
            {
                os << "(items=" << m.items << ", async=" << m.async << ", local=" << m.local <<  ")";
                return os;
            }

            //! items.async.local
            static int compare(const metrics &lhs, const metrics &rhs) throw();

            //! mostly for debug
            inline void print(FILE *fp) const
            {
                assert(fp);
                fprintf(fp,"(items=%g async=%g local=%g)",items.to_double(),async.to_double(),local.to_double());
            }
        private:
            YOCTO_DISABLE_ASSIGN(metrics);
        };


        ////////////////////////////////////////////////////////////////////////
        //
        //! parameters ensuring parallel division is worthy
        //
        ////////////////////////////////////////////////////////////////////////
        YOCTO_PAIR_DECL(STANDARD,cycle_params,metrics::type,w,metrics::type,l);
        typedef metrics::type type;
        inline cycle_params() : w(), l() {}
        inline void compute_from(const metrics &seq, const metrics &par)
        {
            const type dA  = 1;
            const type A   = par.async+1;
            w = (seq.items - par.items)/A;
            l = (seq.local - par.local)/A;
        }
        inline void keep_min(const cycle_params &other)
        {
            if(other.w<w) w=other.w;
            if(other.l<l) l=other.l;
        }
        YOCTO_PAIR_END();


        ////////////////////////////////////////////////////////////////////////
        //
        //! effective rates
        //
        ////////////////////////////////////////////////////////////////////////
        YOCTO_PAIR_DECL(STANDARD,cycle_rates,metrics::type,wxch,metrics::type,copy);
        inline cycle_rates() : wxch(), copy() {}
        void clear() { wxch.ldz(); copy.ldz(); }
        inline void compute_from(const cycle_params &params, const metrics &par )
        {
            wxch = par.items + par.async * params.w;
            copy = par.local + par.async * params.l;
        }
        inline void keep_max(const cycle_rates &other)
        {
            if(wxch<other.wxch) wxch = other.wxch;
            if(copy<other.copy) copy = other.copy;
        }
        inline void print(FILE *fp) const
        {
            assert(fp);
            fprintf(fp,"%g\t+lambda*%g",wxch.to_double(),copy.to_double());
        }
        YOCTO_PAIR_END();

    }
}

#endif

