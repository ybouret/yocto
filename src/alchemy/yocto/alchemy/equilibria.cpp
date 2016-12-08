#include "yocto/alchemy/equilibria.hpp"
#include "yocto/code/utils.hpp"

namespace yocto
{
    namespace alchemy
    {
        equilibria:: ~equilibria() throw() {}

        equilibria:: equilibria(const library::pointer &shlib) :
        equilibrium::database(),
        pLib(shlib),
        M(0),
        N(0),
        Nu(),
        NuT(),
        max_name_length(0)
        {}

        void equilibria::submit(const equilibrium::pointer &eq)
        {
            if( !insert(eq) ) throw exception("alchemy.equilibria(multiple '%s')",eq->name.c_str());
            max_name_length = max_of<size_t>(max_name_length,eq->name.size());
        }
        
        equilibrium & equilibria:: add( const string &name, const equilibrium_constant &K)
        {
            equilibrium::pointer eq( new equilibrium(name,pLib,K) );
            submit(eq);
            return *eq;
        }
        
        equilibrium & equilibria:: add( const string &name, const double K)
        {
            equilibrium::pointer eq( new equilibrium(name,pLib,K) );
            submit(eq);
            return *eq;
        }

        void equilibria:: compile()
        {
            (size_t &)N = size();
            (size_t &)M = pLib->size();
            K.release();
            Gamma.release();
            Phi.release();
            NuT.release();
            Nu.release();
            try
            {
                if(N>0)
                {
                    Nu.make(N,M);
                    NuT.make(M,N);
                    Phi.make(N,M);
                    Gamma.make(N);
                    K.make(N);
                    size_t i = 1;
                    for(iterator it=begin();i<=N;++i,++it)
                    {
                        const equilibrium &eq = **it;
                        for(const actor *node = eq.get_products().head;node;node=node->next)
                        {
                            const size_t j  = node->id;
                            const int    nu = node->nu;
                            Nu[i][j]  = nu;
                            NuT[j][i] = nu;
                        }
                    }
                }
            }
            catch(...)
            {
                (size_t &)N = 0;
                (size_t &)M = 0;
                throw;
            }
        }

        void equilibria:: computeGammaAndPhi(const array<double> &C, const double t)
        {
            size_t i = 1;
            for(iterator it=begin();i<=N;++i,++it)
            {
                const equilibrium &eq = **it;
                Gamma[i] = eq.computeGamma(C, t, K[i]);
                eq.computeGradient(Phi[i], C, K[i]);
            }
        }

    }
}

