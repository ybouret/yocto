
#include "yocto/chemical/boot.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/adjoint.hpp"
#include "yocto/math/core/svd.hpp"

namespace yocto
{
    using namespace math;
    
    namespace chemical
    {

        void  boot:: guess(array<double>    &C0,
                           equilibria       &cs,
                           const double      t,
                           Randomized::Bits &ran)
        {
            //__________________________________________________________________
            //
            // check dimensionality
            //__________________________________________________________________
            const size_t N = cs.N;
            const size_t M = cs.M;
            const size_t Nc = constraints.size();

            if(Nc+N!=M)
            {
                throw exception("boot.%s: #constraints=%u+#equilibria=%u != #species=%u",
                                *name, unsigned(Nc), unsigned(N), unsigned(M) );
            }

            if(N<=0)
            {
                // special case
                throw exception("boot.%s: not handled!", *name);
                return;
            }

            //__________________________________________________________________
            //
            // construct the linear algebra problem
            //__________________________________________________________________
            const matrix<double> &Nu = cs.Nu;
            matrix<double>        P(Nc,M);
            vector<double>        Lam(Nc,0);

            for(size_t k=Nc;k>0;--k)
            {
                array<double>    &Pk = P[k];
                const constraint &c  = *constraints[k];
                Lam[k] = c.fill(Pk);
                for(size_t i=N;i>0;--i)
                {
                    if(Fabs(tao::dot(Pk,Nu[i]))>0)
                    {
                        throw exception("boot.%s: constraint #%u is not orthogonal to equilibrium #%u",*name, unsigned(k), unsigned(i));
                    }
                }

            }
            //std::cerr << "P  =" << P << std::endl;
            //std::cerr << "Lam=" << Lam << std::endl;

            //__________________________________________________________________
            //
            // construct most precise Cstar, checking that the linear
            // problem is consistent
            //__________________________________________________________________
            vector<double> Cstar(M,0);
            {
                matrix<double> P2(Nc,Nc);
                tao::mmul_rtrn(P2,P,P);
                const double   dP2 = determinant(P2);
                if(Fabs(dP2)<=0)
                {
                    throw exception("boot.%s: singular system of constraints", *name);
                }
                matrix<double> aP2(Nc,Nc);
                adjoint(aP2,P2);
                vector<double> U(Nc,0);
                tao::mul(U,aP2,Lam);
                tao::mul_trn(Cstar,P,U);
                tao::divby(dP2,Cstar);
            }
            //std::cerr << "Cstar=" << Cstar << std::endl;

            //__________________________________________________________________
            //
            // construct the balanced concentration from Cstar
            //__________________________________________________________________
            vector<double> Cplus(Cstar);
            if(!cs.balance(Cplus))
            {
                throw exception("boot.%s: no possible balanced solution!", *name);
            }
            //std::cerr << "Cplus=" << Cplus << std::endl;

            vector<double> X(Cplus);
            bool   initialize     = true;
            double amplification  = 1;
            while(!cs.normalize(X,t,initialize))
            {
                initialize = false;
                tao::set(X,Cplus);
                for(size_t i=N;i>0;--i)
                {
                    const double         Cs   = cs.get_scale(i) * amplification;
                    const array<double> &Nu_i = Nu[i];
                    for(size_t j=M;j>0;--j)
                    {
                        if(Fabs(Nu_i[j])>0)
                        {
                            X[j] += Cs * ran.get<double>();
                        }
                    }
                }
                cs.project(X,Cstar);
                amplification *= 1.1;
            }
            tao::set(C0,X,M);
        }
    }

}


