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
        C(),
        dC(),
        active(),
        beta(),
        Nu2(),
        eta(),
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
            eta.   release();
            Nu2.   release();
            beta.  release();
            active.release();
            dC.    release();
            C.     release();
            xi.    release();
            Xi.    release();
            K.     release();
            Gamma. release();
            Phi.   release();
            NuT.   release();
            Nu.    release();
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
                    C.make(M);
                    dC.make(M);
                    active.make(M,false);
                    beta.make(M);
                    Nu2.make(M,M);
                    eta.make(M);

                    size_t i = 1;
                    for(iterator it=begin();i<=N;++i,++it)
                    {
                        const equilibrium &eq = **it;
                        for(const actor *node = eq.get_products().head;node;node=node->next)
                        {
                            const size_t j  = node->id;
                            const int    nu = node->nu; assert(nu>0);
                            Nu[i][j]  = nu;
                            NuT[j][i] = nu;
                            active[j] = true;
                        }
                        for(const actor *node = eq.get_reactants().head;node;node=node->next)
                        {
                            const size_t j  = node->id;
                            const int    nu = node->nu; assert(nu<0);
                            Nu[i][j]  = nu;
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

        void equilibria:: buildXi()
        {
            tao::mmul_rtrn(Xi, Phi, Nu);
            if(!LU<double>::build(Xi))
            {
                throw exception("equilibria: singular composition");
            }
        }

        void equilibria:: computeXi(const array<double> &C0,
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

        void equilibria:: updateXi(const array<double> &C0)
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

#ifndef NDEBUG
            for(size_t i=1;i<=M;++i) { assert(C[i]>=0); }
#endif
            computeXi(C,t);

            size_t count=0;
            while(true)
            {
                //______________________________________________________________
                //
                // compute the raw extent
                //______________________________________________________________
                std::cerr << std::endl;
                std::cerr << "Gamma=" << Gamma << std::endl;
                tao::neg(xi,Gamma);
                LU<double>::solve(Xi,xi);

                std::cerr << "C  =" << C  << std::endl;
                std::cerr << "xi =" << xi << std::endl;



                tao::mul(dC, NuT, xi);
                tao::add(C, dC);
                updateXi(C);
                if(++count>=10) break;
            }

        }





    }

}

namespace yocto
{
    namespace alchemy
    {

        void equilibria:: balance()
        {
            std::cerr << "C=" << C << std::endl;
            while(true)
            {
                //______________________________________________________________
                //
                // counting bad species
                //______________________________________________________________
                size_t nbad = 0;
                for(size_t i=M;i>0;--i)
                {
                    beta[i] = 0;
                    if(active[i]&&C[i]<0)
                    {
                        beta[i] = 1;
                        ++nbad;
                    }
                }
                if(nbad<=0) return;
                std::cerr << "nbad=" << nbad  << std::endl;
                std::cerr << "beta=" << beta << std::endl;

                //______________________________________________________________
                //
                // algebraic eta: descent direction
                //______________________________________________________________
                tao::mul(eta,Nu2,beta);
                std::cerr << "eta=" << eta << std::endl;
                double alpha = 0;
                size_t jzero = 0;

                //______________________________________________________________
                //
                // find greatest alpha
                //______________________________________________________________

                for(size_t j=M;j>0;--j)
                {
                    if(active[j])
                    {
                        //______________________________________________________
                        //
                        // what do we do ?
                        //______________________________________________________
                        const double Cj = C[j];
                        if(Cj<0)
                        {
                            //__________________________________________________
                            //
                            // bad concentration, go to zero if possible
                            //__________________________________________________
                            const int ej = eta[j];
                            if(ej<0)
                            {
                                const string &name = (*pLib)(j)->name;
                                throw exception("impossible to balance '%s'", name.c_str());
                            }
                            else
                            {
                                if(ej>0)
                                {
                                    const double atmp = -Cj/ej;
                                    if(alpha>0)
                                    {
                                        if(atmp<alpha)
                                        {
                                            alpha = atmp;
                                            jzero = j;
                                            std::cerr << "updating jzero=" << jzero << "; alpha=" << alpha << std::endl;
                                        }
                                    }
                                    else
                                    {
                                        jzero = j;
                                        alpha = atmp;
                                        std::cerr << "initializing jzero=" << jzero << "; alpha=" << alpha << std::endl;
                                    }
                                }
                                // else if eta[j] = 0: do nothing
                            }
                        }
                        else
                        {
                            //__________________________________________________
                            //
                            // good concentration, don't go too low !
                            //__________________________________________________
                        }

                    }
                }
                break;
            }
        }
        
    }
    
}


