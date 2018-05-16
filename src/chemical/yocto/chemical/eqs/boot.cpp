
#include "yocto/chemical/boot.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/adjoint.hpp"
#include "yocto/math/core/svd.hpp"
#include "yocto/math/opt/minimize.hpp"
#include "yocto/math/opt/bracket.hpp"

namespace yocto
{
    using namespace math;
    
    namespace chemical
    {

        double boot:: __Balance(double alpha) throw()
        {
            double E = 0;
            for(size_t j=M;j>0;--j)
            {
                beta[j] = 0;
                const double Cj  = (Ctry[j] = Corg[j] + alpha * dC[j]);
                const double Cj2 = Cj*Cj;
                if(Cj2<=0)
                {
                    Ctry[j] = 0;
                }
                else if( Cj < 0 )
                {
                    beta[j] = -Cj;
                    E += Cj2;
                }
            }
            return E;
        }

        static inline
        void __optimize(math::numeric<double>::function &F,
                        triplet<double>                 &XX,
                        triplet<double>                 &FF) throw()
        {

            double width = XX.c-XX.a; assert(width>=0);
            while(true)
            {
                kernel::minimize(F, XX, FF);
                const double new_width = XX.c-XX.a;
                assert(new_width<=width);
                if(new_width>=width)
                {
                    break;
                }
                width = new_width;
            }
        }

        void  boot:: guess(array<double>    &C0,
                           equilibria       &cs,
                           const double      t)
        {
            clear();
            try
            {
                //__________________________________________________________________
                //
                // check dimensionality
                //__________________________________________________________________
                N = cs.N;
                M = cs.M;
                Nc = constraints.size();

                if(Nc+N!=M)
                {
                    throw exception("boot.%s: #constraints=%u+#equilibria=%u != #species=%u",
                                    *name, unsigned(Nc), unsigned(N), unsigned(M) );
                }

                if(N<=0)
                {
                    // special case
                    throw exception("boot.%s: no equilibrium not handled!", *name);
                    return;
                }

                if(Nc<=0)
                {
                    //special case
                    throw exception("boot.%s: no constraints not handled!", *name);
                    return;
                }

                //______________________________________________________________
                //
                // allocated workspace
                //______________________________________________________________
                P.    make(Nc,M);
                L.    make(Nc,0);
                Cstar.make(M,0);
                Q.    make(N,M);
                Corg. make(M);
                Ctry. make(M);
                beta. make(M);
                dC  . make(M);
                V   . make(N);

                //______________________________________________________________
                //
                // construct the linear algebra problem
                //______________________________________________________________
                for(size_t k=Nc;k>0;--k)
                {
                    array<double>    &Pk = P[k];
                    const constraint &c  = *constraints[k];
                    L[k] = c.fill(Pk);
                }

                std::cerr << "P=" << P << std::endl;
                std::cerr << "L=" << L << std::endl;

                //______________________________________________________________
                //
                // construct most precise Cstar, checking that the linear
                // problem is consistent
                //______________________________________________________________
                {
                    matrix<double> P2(Nc,Nc);
                    tao::mmul_rtrn(P2,P,P);
                    const double   dP2 = ideterminant(P2);
                    if(Fabs(dP2)<=0)
                    {
                        throw exception("boot.%s: singular system of constraints", *name);
                    }
                    matrix<double> aP2(Nc,Nc);
                    iadjoint(aP2,P2);

                    vector<double> U(Nc,0);
                    tao::mul(U,aP2,L);
                    tao::mul_trn(Cstar,P,U);
                    tao::divby(dP2,Cstar);
                    std::cerr << "aP2=" << aP2 << std::endl;
                    std::cerr << "dP2=" << dP2 << std::endl;
                    std::cerr << "U  =" << U   << std::endl;
                }
                std::cerr << "Cstar=" << Cstar << std::endl;

                //______________________________________________________________
                //
                // construct one orthonormal subspace
                //______________________________________________________________
                if(! svd<double>::orthonormal(Q,P) )
                {
                    throw exception("boot.%s: unable to find boot space", *name);
                }
                for(size_t i=N;i>0;--i)
                {
                    array<double> &Qi = Q[i];
                    if( svd<double>::truncate(Qi) > 0 )
                    {
                        tao::divby( tao::norm(Qi), Qi);
                    }
                }
                std::cerr << "Q=" << Q << std::endl;

                // initial point
                tao::set(Corg,Cstar);

                balance(cs.active);



            }
            catch(...)
            {
                clear(); throw;
            }
            clear();
        }

        void boot:: balance(const array<bool> &active)
        {

            // compute initial value and beta from Corg
            double E0 = __Balance(0.0);
            if(E0<=0)
            {
                std::cerr << "boot.balanced..." << std::endl;
                return;
            }

            while(true)
            {
                // Corg, beta and E0 are computed
                std::cerr << "E0  =" << E0   << std::endl;
                std::cerr << "Corg=" << Corg << std::endl;
                std::cerr << "beta=" << beta << std::endl;
                tao::mul(V,Q,beta);
                tao::mul_trn(dC,Q,V);
                std::cerr << "dC=" << dC << std::endl;
                double alpha = 1;
                double E1    = __Balance(alpha);
                if(E1<=0)
                {
                    break;
                }

                triplet<double> xx = { 0, alpha, alpha };
                triplet<double> ee = { E0, E1, E1 };
                bracket<double>::expand(Balance,xx,ee);
                xx.co_sort(ee);
                __optimize(Balance,xx,ee);
                E1 = __Balance(alpha=xx.b);
                std::cerr << "E1=" << E1 << std::endl;
                if(E1<=0||E1>=E0)
                {
                    break;
                }

                // new step
                E0 = E1;
                tao::set(Corg,Ctry);
            }
            // out of the loop
            tao::set(Corg,Ctry);
            for(size_t j=M;j>0;--j)
            {
                if(active[j])
                {
                    Corg[j] = max_of<double>(0.0,Ctry[j]);
                }
                else
                {
                    Corg[j] = Ctry[j];
                }
            }
            std::cerr << "C=" << Corg << std::endl;
        }

    }

}


