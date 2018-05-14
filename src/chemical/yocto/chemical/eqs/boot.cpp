
#include "yocto/chemical/boot.hpp"


namespace yocto
{
    namespace chemical
    {
        void  boot:: guess(array<double> &C0,
                           equilibria    &cs,
                           const double   t)
        {
            const size_t N = cs.N;
            const size_t M = cs.M;
            const size_t Nc = constraints.size();

            if(Nc+N!=M)
            {
                throw exception("boot.%s: #constraints=%u+#equilibria=%u != #species=%u", *name, unsigned(Nc), unsigned(N), unsigned(M) );
            }

            if(N<=0)
            {
                // special case
                return;
            }

            matrix<double> P(Nc,M);
            vector<double> Lambda(Nc,0);
            for(size_t k=Nc;k>0;--k)
            {
                const constraint &c = *constraints[k];
                Lambda[k] = c.fill(P[k]);
            }
            std::cerr << "P=" << P << std::endl;
            std::cerr << "Lam=" << Lambda << std::endl;
        }
    }

}


