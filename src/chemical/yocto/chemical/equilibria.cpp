
#include "yocto/chemical/equilibria.hpp"
#include "yocto/exception.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/lu.hpp"
#include "yocto/math/core/adjoint.hpp"
#include "yocto/sort/index.hpp"

namespace yocto
{
    using namespace math;
    
    namespace chemical
    {
        equilibria:: ~equilibria() throw() {}

        equilibria::equilibria(const string &id, const size_t n) :
        equilibrium::database(n,as_capacity),
        name(id),
        M(0),
        N(0),
        C(),
        dC(),
        active(),
        Ctry(),

        Cini(),
        Cend(),
        peqs(),
        beta(),
        Nu(),
        NuT(),
        nu2(),
        Phi(),
        W(),
        K(),
        Gamma(),
        xi(),
        gev(),
        max_length(0),

        project(this, & equilibria:: __Project),

        pEq(0),
        KEq(0),
        zsolve(0),

        NormGamma(this, & equilibria::__NormGamma),
        Balance(this,   & equilibria::__Balance  ),
        GammaEq(this,   & equilibria::__GammaEq  )
        {
        }

        void equilibria:: __Project(array<double> &C0, const double t)
        {
            if( !normalize(C0,t,true) )
            {
                throw exception("equilibria: invalid concentration @t=%g",t);
            }
        }

        equilibrium & equilibria:: operator()(const string &id, const double K)
        {
            equilibrium::pointer eq( new constant_equilibrium(id,K) );
            if(!insert(eq))
            {
                throw exception("%s: multiple '%s'", *name, *id);
            }
            return *eq;
        }

        std::ostream & equilibria:: spaces_for( const string &id, std::ostream &os ) const
        {
            const size_t eq_len = id.length();
            const size_t mx_len = max_length;
            for(size_t i=eq_len;i<=mx_len;++i)
            {
                os << ' ';
            }
            return os;
        }


        std::ostream & operator<<( std::ostream &os, const equilibria &eqs)
        {
            os << "<equilibria:" << eqs.name << "#" << eqs.size() << ">" << std::endl;
            for( equilibria::const_iterator i=eqs.begin();i!=eqs.end();++i)
            {
                const equilibrium &eq = **i;
                os << '\t';// << **i << std::endl;
                const size_t eq_len = eq.name.length();
                const size_t mx_len = eqs.max_length;
                if(mx_len>=eq_len)
                {
                    eq.display(os,mx_len-eq_len);
                }
                else
                {
                    eq.display(os,0);
                }
                os << std::endl;
            }
            os << "<equilibria:" << eqs.name << "/>";
            return os;
        }

        void equilibria::clear() throw()
        {
            (size_t &)M   = 0;
            (size_t &)N   = 0;
            (size_t &)max_length = 0;
            dNu2          = 0;
            
            gev.    release();
            xi.     release();
            Gamma.  release();
            K.      release();
            W.      release();
            Phi.    release();
            nu2.    release();
            aNu2.   release();
            Nu2.    release();
            NuT.    release();
            Nu.     release();
            errc.   release();
            beta.   release();
            peqs.   release();
            step.   release();
            Cend.   release();
            Cini.   release();

            Ctry.   release();
            active. release();
            dC.     release();
            C.      release();
        }


        void equilibria:: compile_for(const library &lib)
        {
            clear();

            // check equilibria
            size_t &m = (size_t &)max_length;
            for(iterator i=begin();i!=end();++i)
            {
                const equilibrium &eq = **i;
                eq.compile();
                m = max_of(m,eq.name.length());
            }

            // compute sizes
            (size_t &)M = lib.size();
            (size_t &)N = size();
            if(M<=0) throw exception("empty library!");

            try
            {
                C.      make(M);
                dC.     make(M);
                active. make(M,false);
                Ctry.   make(M);
                beta.   make(M);
                errc.   make(M);

                if(N>0)
                {
                    Cini.  make(M);
                    Cend.  make(M);
                    step.  make(M);
                    peqs.  make(N);
                    Nu.    make(N,M);
                    NuT.   make(M,N);
                    Nu2.   make(N,N);
                    aNu2.  make(N,N);
                    nu2.   make(N,0);
                    Phi.   make(N,M);
                    W.     make(N);
                    K.     make(N);
                    Gamma. make(N);
                    xi.    make(N);
                    gev.   make(N);

                    //__________________________________________________________
                    //
                    // Compute Topology Matrix Nu
                    //__________________________________________________________
                    {
                        size_t ii=1;
                        for(iterator i=begin();i!=end();++i,++ii)
                        {
                            equilibrium &eq = **i;
                            eq.fill(Nu[ii],active);

                            peqs[ii] = &eq;
                            nu2[ii]  = eq.nu2; assert(nu2[ii]>0);
                            gev[ii]  = 1.0/sqrt(nu2[ii]);
                        }
                    }

                    //__________________________________________________________
                    //
                    // compute its transpose
                    //__________________________________________________________
                    for(size_t i=1;i<=N;++i)
                    {
                        const array<double> &Nu_i = Nu[i];
                        for(size_t j=1;j<=M;++j)
                        {
                            NuT[j][i] = Nu_i[j];
                        }
                    }


                    //__________________________________________________________
                    //
                    // check OK
                    //__________________________________________________________
                    tao::mmul(Nu2,Nu,NuT);
                    dNu2 = ideterminant(Nu2);
                    if( Fabs(dNu2) <= 0)
                    {
                        throw exception("singular system of chemical equations");
                    }
                    iadjoint(aNu2,Nu2);

                }

            }
            catch(...)
            {
                clear();
                throw;
            }
        }

#define __COMPUTE_Kt() \
const double Kt = (K[i] = max_of<double>(eq.K(t),0))
        
        void equilibria:: initializeGamma(const array<double> &C0, const double t)
        {
            for(size_t i=N;i>0;--i)
            {
                const equilibrium &eq = *peqs[i]; assert(eq.nu2>0); assert(nu2[i]>0);
                __COMPUTE_Kt();
                Gamma[i] = eq.computeGamma(C0,Kt);
            }
        }

        void equilibria:: initializeGammaAndPhi(const array<double> &C0,const double t)
        {
            for(size_t i=N;i>0;--i)
            {
                const equilibrium &eq = *peqs[i];
                __COMPUTE_Kt();
                Gamma[i] = eq.computeGamma(C0,Kt);
                eq.computeGradient(Phi[i],C0,Kt);
            }
        }

        void equilibria:: updateGamma(const array<double> &C0)
        {
            for(size_t i=N;i>0;--i)
            {
                const equilibrium &eq = *peqs[i];
                Gamma[i] = eq.computeGamma(C0,K[i]);
            }
        }



        void equilibria:: updatePhi(const array<double> &C0)
        {
            for(size_t i=N;i>0;--i)
            {
                const equilibrium &eq = *peqs[i];
                const double       Kt = K[i];
                eq.computeGradient(Phi[i],C0,Kt);
            }
        }

        void equilibria:: updateGammaAndPhi(const array<double> &C0)
        {
            for(size_t i=N;i>0;--i)
            {
                const equilibrium &eq = *peqs[i];
                const double       Kt = K[i];
                Gamma[i] = eq.computeGamma(C0,Kt);
                eq.computeGradient(Phi[i],C0,Kt);
            }
        }
        
        
        double equilibria:: GammaToScalar() const throw()
        {
            double ans = 0;
            for(size_t i=N;i>0;--i)
            {
                ans += pow( Fabs(Gamma[i]), gev[i] );
            }
            return ans;
        }

        double equilibria:: get_scale( const size_t iEq ) const throw()
        {
            assert(iEq>=1);
            assert(iEq<=N);
            return peqs[iEq]->scale( K[iEq] );
        }

        bool equilibria:: deliver(array<double>       &C0,
                                  const array<double> &delta,
                                  const double         t,
                                  const bool           initialize) throw()
        {
            //__________________________________________________________________
            //
            // starting point
            //__________________________________________________________________
            if(initialize)
            {
                initializeGammaAndPhi(C0,t);
            }
            else
            {
                updateGammaAndPhi(C0);
            }

            //__________________________________________________________________
            //
            // chemical moderation
            //__________________________________________________________________
            tao::mmul_rtrn(W,Phi,Nu);
            if(!LU<double>::build(W))
            {
                return false; // bad...
            }
            tao::set(xi,Gamma);
            tao::mul_add(xi,Phi,delta);
            LU<double>::solve(W,xi);
            tao::mul(dC,NuT,xi);
            
            for(size_t j=M;j>0;--j)
            {
                C0[j] += (delta[j] - dC[j]);
            }
            return normalize(C0,0,false);
        }

        bool equilibria:: damp( array<double> &delta, const array<double> &C0, const double t, const bool initialize) throw()
        {
            //__________________________________________________________________
            //
            // starting point
            //__________________________________________________________________
            if(initialize)
            {
                initializeGammaAndPhi(C0,t);
            }
            else
            {
                updateGammaAndPhi(C0);
            }

            //__________________________________________________________________
            //
            // chemical moderation
            //__________________________________________________________________
            tao::mmul_rtrn(W,Phi,Nu);
            if(!LU<double>::build(W))
            {
                return false; // bad...
            }
            tao::set(xi,Gamma);
            tao::mul_add(xi,Phi,delta);
            LU<double>::solve(W,xi);
            tao::mul(dC,NuT,xi);
            for(size_t j=M;j>0;--j)
            {
                delta[j] -= dC[j];
            }
            return true;
        }


    }
}
