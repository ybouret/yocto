#include "yocto/alchemy/equilibria.hpp"
#include "yocto/code/utils.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/lu.hpp"
#include "yocto/math/core/adjoint.hpp"

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
        g(),
        h(),
        iNu(),
        E(this, & equilibria::call_E),
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
            iNu.   release();
            b.     release();
            h.     release();
            g.     release();
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
                    g.     make(M);
                    h.     make(M);
                    b.     make(M);
                    iNu.   make(N,M);

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
                    //std::cerr << "Nu =" << Nu  << std::endl;
                    //std::cerr << "Nu2=" << Nu2 << std::endl;

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
            tao::mmul_rtrn(Chi, Phi, Nu);
            if(!LU<double>::build(Chi))
            {
                throw exception("equilibria: singular composition");
            }
        }

        void equilibria:: computeChi(const array<double> &C0,
                                      const double         t)
        {
            size_t i = 1;
            for(iterator it=begin();i<=N;++i,++it)
            {
                const equilibrium &eq = **it;
                Gamma[i] = eq.computeGamma(C0, t, K[i]);
                eq.computeGradient(Phi[i], C0,    K[i]);
            }
            buildXi();
        }

        void equilibria:: updateChi(const array<double> &C0)
        {
            size_t i=1;
            for(iterator it=begin();i<=N;++i,++it)
            {
                const equilibrium &eq = **it;
                const double Kt = K[i];
                Gamma[i] = eq.updateGamma(C0,Kt);
                eq.computeGradient(Phi[i],C0,Kt);
            }
            buildXi();
        }

    }
}

#include "yocto/code/utils.hpp"

namespace yocto
{
    namespace alchemy
    {

        void equilibria:: normalize(array<double> &C0, const double t)
        {
            //__________________________________________________________________
            //
            // initialize
            //__________________________________________________________________
            assert(C0.size()>=M);
            tao::set(C,C0);
            balance();

            computeChi(C,t);

            double norm2=0;
            bool   check=false;

            while(true)
            {
                //______________________________________________________________
                //
                // compute the raw extent
                //______________________________________________________________
                std::cerr << std::endl;
                std::cerr << "Gamma=" << Gamma << std::endl;
                tao::neg(xi,Gamma);
                LU<double>::solve(Chi,xi);

                std::cerr << "C  =" << C  << std::endl;
                std::cerr << "xi =" << xi << std::endl;



                tao::mul(dC, NuT, xi);
                tao::add(C, dC);
                balance();
                updateChi(C);
                const double temp2 = tao::norm_sq(dC);
                if(check)
                {
                    std::cerr << "norm=" << temp2 << "/" << norm2 << std::endl;
                    if(temp2>=norm2)
                    {
                        std::cerr << "Reached equilibrium" << std::endl;
                        // set back value
                        for(size_t i=M;i>0;--i)
                        {
                            C0[i] = C[i];
                        }
                        break;
                    }
                    norm2 = temp2;
                }
                else
                {
                    check = true;
                    norm2 = temp2;
                }
                //if(++count>=10) break;
            }
            
        }
        
        
        
        
        
    }

}


