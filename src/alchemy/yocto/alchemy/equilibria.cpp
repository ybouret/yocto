#include "yocto/alchemy/equilibria.hpp"
#include "yocto/code/utils.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/lu.hpp"

namespace yocto
{
    using namespace math;

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
        Phi(),
        Gamma(),
        K(),
        Xi(),
        xi(),
        dC(),
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
            dC.   release();
            xi.   release();
            Xi.   release();
            K.    release();
            Gamma.release();
            Phi.  release();
            NuT.  release();
            Nu.   release();
            try
            {
                if(N>0)
                {
                    Nu.make(N,M);
                    NuT.make(M,N);
                    Phi.make(N,M);
                    Gamma.make(N);
                    K.make(N);
                    Xi.make(N);
                    xi.make(N);
                    dC.make(M);
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
                        for(const actor *node = eq.get_reactants().head;node;node=node->next)
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

        void equilibria:: buildXi()
        {
            tao::mmul_rtrn(Xi, Phi, Nu);
            if(!LU<double>::build(Xi))
            {
                throw exception("equilibria: singular composition");
            }
        }

        void equilibria:: computeXi(const array<double> &C, const double t)
        {
            size_t i = 1;
            for(iterator it=begin();i<=N;++i,++it)
            {
                const equilibrium &eq = **it;
                Gamma[i] = eq.computeGamma(C, t, K[i]);
                eq.computeGradient(Phi[i], C,    K[i]);
            }
            buildXi();
        }

        void equilibria:: updateXi(const array<double> &C)
        {
            size_t i=1;
            for(iterator it=begin();i<=N;++i,++it)
            {
                const equilibrium &eq = **it;
                const double Kt = K[i];
                Gamma[i] = eq.updateGamma(C,Kt);
                eq.computeGradient(Phi[i],C,Kt);
            }
            buildXi();
        }

    }
}


namespace yocto
{
    namespace alchemy
    {

        void equilibria:: normalize(array<double> &C, const double t)
        {
            //__________________________________________________________________
            //
            // initialize
            //__________________________________________________________________
            assert(C.size()>=M);
#ifndef NDEBUG
            for(size_t i=1;i<=M;++i) { assert(C[i]>=0); }
#endif
            computeXi(C,t);

            while(true)
            {
                //______________________________________________________________
                //
                // compute the raw extent
                //______________________________________________________________
                tao::neg(xi,Gamma);
                LU<double>::solve(Xi,xi);

                std::cerr << "C=" << C << std::endl;
                std::cerr << "xi=" << xi << std::endl;
                tao::mul(dC, NuT, xi);
                std::cerr << "dC=" << dC << std::endl;
                

                break;
            }


        }


    }

}


