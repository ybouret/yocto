
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
                           const double      t)
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
            vector<double>        L(Nc,0);

            for(size_t k=Nc;k>0;--k)
            {
                array<double>    &Pk = P[k];
                const constraint &c  = *constraints[k];
                L[k] = c.fill(Pk);
                for(size_t i=N;i>0;--i)
                {
                    if(Fabs(tao::dot(Pk,Nu[i]))>0)
                    {
                        throw exception("boot.%s: constraint #%u is not orthogonal to equilibrium #%u",*name, unsigned(k), unsigned(i));
                    }
                }
            }

            std::cerr << "P=" << P << std::endl;
            std::cerr << "L=" << L << std::endl;
            matrix<double> P2(Nc,Nc);
            tao::mmul_rtrn(P2,P,P);
            const double   dP2 = determinant(P2);
            if(Fabs(dP2)<=0)
            {
                throw exception("boot.%s: singular system of constraints", *name);
            }
            matrix<double> aP2(Nc,Nc);
            adjoint(aP2,P2);

            vector<double> X(M);
            vector<double> dL(Nc);
            vector<double> U(Nc);
            vector<double> dX(M);

            if(!cs.normalize(X,t))
            {
                std::cerr << "cannot normalize X" << std::endl;
                exit(0);
            }
            std::cerr << "X=" << X << std::endl;
            tao::mul(dL,P,X);
            tao::subp(dL,L);   std::cerr << "dL=" << dL << std::endl;
            tao::mul(U,aP2,dL);
            tao::mul_trn(dX,P,U);
            tao::divby(dP2,dX);
            std::cerr << "dX=" << dX << std::endl;

            exit(0);

#if 0
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
                tao::mul(U,aP2,L);
                tao::mul_trn(Cstar,P,U);
                tao::divby(dP2,Cstar);
            }
            std::cerr << "Cstar=" << Cstar << std::endl;

            if(!cs.balance(Cstar))
            {
                throw exception("boot.%s: unable to balance Cstar",*name);
            }


            if(!cs.normalize(Cstar,t))
            {
                throw exception("boot.%s: unable to solve",*name);
            }
            std::cerr << "Cfinal=" << Cstar << std::endl;
            tao::set(C0,Cstar,M);
#endif

        }
    }

}


