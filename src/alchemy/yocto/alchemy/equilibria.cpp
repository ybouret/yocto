#include "yocto/alchemy/equilibria.hpp"
#include "yocto/code/utils.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/svd.hpp"

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
        Nu2(),
        Phi(),
        Gamma(),
        K(),
        Chi(),
        xi(),
        C(),
        dC(),
        active(),
        beta(),
        Cini(),
        step(),
        g(),
        h(),
        b(),
        iNu(),
        nuP(),
        gam(),
        drvs(),
        E(this, & equilibria::call_E),
        F(this, & equilibria::call_F),
        max_name_length(0)
        {}

        std::ostream & operator<<( std::ostream &os, const equilibria &eqs)
        {
            for( equilibria::const_iterator i = eqs.begin(); i != eqs.end(); ++i)
            {
                const equilibrium &eq = **i;
                os << eq.name; for(size_t j=eq.name.size();j<=eqs.max_name_length;++j) os << ' '; os << ':'; os << ' ';
                os << eq << std::endl;
            }
            return os;
        }

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
            gam.   release();
            nuP.   release();
            iNu.   release();
            b.     release();
            h.     release();
            g.     release();
            step.  release();
            Cini.  release();
            beta.  release();
            active.release();
            dC.    release();
            C.     release();
            xi.    release();
            Chi.   release();
            K.     release();
            Gamma. release();
            Phi.   release();
            Nu2.   release();
            NuT.   release();
            Nu.    release();
            try
            {
                if(N>0)
                {
                    //__________________________________________________________
                    //
                    // allocate memory
                    //__________________________________________________________
                    Nu.    make(N,M);
                    NuT.   make(M,N);
                    Nu2.   make(M,M);
                    Phi.   make(N,M);
                    Gamma. make(N);
                    K.     make(N);
                    Chi.   make(N);
                    xi.    make(N);
                    C.     make(M);
                    dC.    make(M);
                    active.make(M,false);
                    beta.  make(M);
                    Cini.  make(M);
                    step.  make(M);
                    g.     make(M);
                    h.     make(M);
                    b.     make(M);
                    iNu.   make(N,M);
                    nuP.   make(N);
                    gam.   make(N);

                    //__________________________________________________________
                    //
                    // fill matrices and informations
                    //__________________________________________________________
                    size_t i = 1;
                    for(iterator it=begin();i<=N;++i,++it)
                    {
                        const equilibrium &eq = **it;
                        for(const actor *node = eq.get_products().head;node;node=node->next)
                        {
                            const size_t j  = node->id;
                            const int    nu = node->nu; assert(nu>0);
                            Nu[i][j]  = nu;
                            iNu[i][j] = nu;
                            NuT[j][i] = nu;
                            active[j] = true;
                            nuP[i] += nu;
                        }
                        for(const actor *node = eq.get_reactants().head;node;node=node->next)
                        {
                            const size_t j  = node->id;
                            const int    nu = node->nu; assert(nu<0);
                            Nu[i][j]  = nu;
                            iNu[i][j] = nu;
                            NuT[j][i] = nu;
                            active[j] = true;
                        }
                    }

                    tao::mmul(Nu2, NuT, Nu);
                }
            }
            catch(...)
            {
                (size_t &)N = 0;
                (size_t &)M = 0;
                throw;
            }
        }

       
    }
}

#include "yocto/sort/quick.hpp"

namespace yocto
{
    namespace alchemy
    {

        void equilibria:: validate( array<double> &C0 ) const throw()
        {
            assert(C0.size()>=M);
            for(size_t i=M;i>0;--i)
            {
                if(active[i])
                {
                    C0[i] = max_of<double>(0.0,C0[i]);
                }
            }

        }


        double equilibria:: Gamma2Value() const throw()
        {
            for(size_t i=N;i>0;--i)
            {
                const double g = Fabs(Gamma[i]); //!< value of Gamma
                const double p = nuP[i];         //!< its unit in power of standard conc
                if(p>=2.0)
                {
                    gam[i] = pow(g,1.0/p);
                }
                else
                {
                    gam[i] = g;
                }
            }
            quicksort(gam);
            return tao::sum(gam);
        }
        
        
    }

}


