
#include "yocto/chemical/boot.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/adjoint.hpp"
#include "yocto/math/core/svd.hpp"

namespace yocto
{
    using namespace math;
    
    namespace chemical
    {
        void boot:: clear() throw()
        {
            (size_t&)Nc = 0;
            Cstar. release();
            Lam.   release();
            P.     release();

        }


        void boot:: compute_Cstar()
        {
            matrix<double> P2(Nc,Nc);
            tao::mmul_rtrn(P2,P,P);
            std::cerr << "P2=" << P2 << std::endl;
            const double dP2 = determinant(P2);
            std::cerr << "dP2=" << dP2 << std::endl;
            if(Fabs(dP2)<=0)
            {
                throw exception("boot.%s: singular system of constraints", *name);
            }
            matrix<double> aP2(Nc,Nc);
            adjoint(aP2,P2);
            std::cerr << "aP2=" << aP2 << std::endl;
            vector<double> U(Nc,0);
            tao::mul(U,aP2,Lam);
            tao::mul_trn(Cstar,P,U);
            tao::divby(dP2,Cstar);
        }

        void  boot:: guess(array<double> &C0,
                           equilibria    &cs,
                           const double   t)
        {
            clear();
            try
            {
                const size_t N = cs.N;
                const size_t M = cs.M;
                ( size_t & ) Nc = constraints.size();

                if(Nc+N!=M)
                {
                    throw exception("boot.%s: #constraints=%u+#equilibria=%u != #species=%u", *name, unsigned(Nc), unsigned(N), unsigned(M) );
                }

                if(N<=0)
                {
                    // special case
                    throw exception("boot.%s: not handled!", *name);
                    return;
                }

                // construct the linear algebra
                const matrix<double> &Nu = cs.Nu;
                P.make(Nc,M);
                Lam.make(Nc,0);
                Cstar.make(M,0);

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
                std::cerr << "P=" << P << std::endl;
                std::cerr << "Lam=" << Lam << std::endl;

                // construct most precise Cstar
                compute_Cstar();
                std::cerr << "Cstar=" << Cstar << std::endl;


                vector<double> X(M,0);
                tao::set(X,Cstar);
                if(!cs.balance(X))
                {
                    throw exception("boot.%s: no possible balanced solution", *name);
                }
                std::cerr << "X=" << X << std::endl;
                std::cerr << "Nu=" << Nu << std::endl;
                vector<double> V(N,0);
                cs.compute_extent(V,X,Cstar);
                std::cerr << "V=" << V << std::endl;

                vector<double> Y(M,0.1);
                tao::add(Y,X);
                std::cerr << "Y0=" << Y << std::endl;
                cs.compute_extent(V,Y,Cstar);
                tao::set(Y,Cstar);
                tao::mul_add(Y,cs.NuT,V);
                std::cerr << "Y1=" << Y << std::endl;
                std::cerr << "V1=" << V << std::endl;


            }
            catch(...)
            {
                clear(); throw;
            }
            clear();
        }
    }

}


