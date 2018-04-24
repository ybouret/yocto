
#include "yocto/chemical/equilibria.hpp"
#include "yocto/exception.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/lu.hpp"

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
        N(0)
        {
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

        std::ostream & operator<<( std::ostream &os, const equilibria &eqs)
        {
            os << "<equilibria:" << eqs.name << "#" << eqs.size() << ">" << std::endl;
            for( equilibria::const_iterator i=eqs.begin();i!=eqs.end();++i)
            {
                os << '\t' << **i << std::endl;
            }
            os << "<equilibria:" << eqs.name << "/>";
            return os;
        }

        void equilibria::clear() throw()
        {
            (size_t &)M = 0;
            (size_t &)N = 0;

            GamEV.  release();
            xi.     release();
            Gamma.  release();
            K.      release();
            W.      release();
            Phi.    release();
            nu2.    release();
            nuT.    release();
            nu.     release();
            beta.   release();

            Ctry.   release();
            active. release();
            dC.     release();
            C.      release();
        }


        void equilibria:: compile_for(const library &lib)
        {
            clear();

            // check equilibria
            for(iterator i=begin();i!=end();++i)
            {
                (**i).check();
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

                if(N>0)
                {
                    nu.   make(N,M);
                    nuT.  make(M,N);
                    nu2.  make(M,M);
                    Phi.  make(N,M);
                    W.    make(N);
                    K.    make(N);
                    Gamma.make(N);
                    xi.   make(N);
                    GamEV. make(N);

                    {
                        size_t ii=1;
                        for(iterator i=begin();i!=end();++i,++ii)
                        {
                            const equilibrium &eq = **i;
                            eq.fill(nu[ii],active);
                            const int nuP = eq.productsStoichiometry();
                            if(nuP>0)
                            {
                                GamEV[ii] = 1.0/nuP;
                            }
                            else
                            {
                                GamEV[ii] = 1.0;
                            }
                        }
                    }
                    for(size_t i=1;i<=N;++i)
                    {
                        for(size_t j=1;j<=M;++j)
                        {
                            nuT[j][i] = nu[i][j];
                        }
                    }
                    tao::mmul(nu2, nuT, nu);
                }


                
            }
            catch(...)
            {
                clear();
                throw;
            }
        }

        void equilibria:: initializeGamma(const array<double> &C0, const double t)
        {
            iterator ii=begin();
            for(size_t i=1;i<=N;++i,++ii)
            {
                const equilibrium &eq = **ii;
                Gamma[i] = eq.computeGamma(C0,K[i]=eq.K(t));
            }
        }

        void equilibria:: updateGamma(const array<double> &C0)
        {
            iterator ii=begin();
            for(size_t i=1;i<=N;++i,++ii)
            {
                const equilibrium &eq = **ii;
                Gamma[i] = eq.computeGamma(C0,K[i]);
            }
        }

        void equilibria:: initializeGammaAndPhi(const array<double> &C0,const double t)
        {
            iterator ii=begin();
            for(size_t i=1;i<=N;++i,++ii)
            {
                const equilibrium &eq = **ii;
                const double Kt = (K[i]=eq.K(t));
                Gamma[i] = eq.computeGamma(C0,Kt);
                eq.computeGradient(Phi[i],C0,Kt);
            }
        }

        void equilibria:: updateGammaAndPhi(const array<double> &C0)
        {
            iterator ii=begin();
            for(size_t i=1;i<=N;++i,++ii)
            {
                const equilibrium &eq = **ii;
                const double Kt = K[i];
                Gamma[i] = eq.computeGamma(C0,Kt);
                eq.computeGradient(Phi[i],C0,Kt);
            }
        }

        bool equilibria:: computeW()
        {
            tao::mmul_rtrn(W,Phi,nu);
            return LU<double>::build(W);
        }

        double equilibria:: GammaToScalar() const throw()
        {
            double ans = 0;
            for(size_t i=N;i>0;--i)
            {
                ans += pow( Square(Gamma[i]), GamEV[i] );
            }
            return sqrt(ans);
        }


    }
}
