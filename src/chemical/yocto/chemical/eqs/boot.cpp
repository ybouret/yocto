
#include "yocto/chemical/boot.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/adjoint.hpp"
#include "yocto/math/core/svd.hpp"
#include "yocto/math/opt/minimize.hpp"
#include "yocto/math/opt/bracket.hpp"
#include "yocto/math/core/lu.hpp"

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
                const double Cj  = (Xtry[j] = Xorg[j] + alpha * dX[j]);
                const double Cj2 = Cj*Cj;
                if(Cj2<=0)
                {
                    Xtry[j] = 0;
                }
                else if( Cj < 0 )
                {
                    beta[j] = -Cj;
                    E += Cj2;
                }
            }
            return E;
        }

        double boot:: RMS( const array<double> &XX ) throw()
        {
            tao::mul(dL,P,XX);
            double sum2 = 0;
            for(size_t k=Nc;k>0;--k)
            {
                const double dl = dL[k] - L[k];
                sum2 += (dl*dl)/p2[k];
            }
            return sqrt(sum2/Nc);
        }


        double boot:: __Control(double alpha)
        {
            tao::mulset(beta,alpha,dX);
            tao::set(Xtry,Xorg);
            if(!eqs->deliver(Xtry,beta,0,false))
            {
                throw exception("boot:%s: unable to perform control!", *name);
            }
            return RMS(Xtry);
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
                N   = cs.N;
                M   = cs.M;
                Nc  = constraints.size();
                eqs = &cs;
                
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
                aP2.  make(Nc,Nc);
                Xstar.make(M,0);
                Q.    make(N,M);
                Xorg. make(M);
                Xtry. make(M);
                beta. make(M);
                dX  . make(M);
                V   . make(N);
                dL  . make(Nc);
                U   . make(Nc);
                p2.   make(Nc);

                //______________________________________________________________
                //
                // construct the linear algebra problem
                //______________________________________________________________
                for(size_t k=Nc;k>0;--k)
                {
                    array<double>    &Pk = P[k];
                    const constraint &c  = *constraints[k];
                    L[k]  = c.fill(Pk);
                    p2[k] = tao::norm_sq(Pk);
                }

                std::cerr << "P=" << P << std::endl;
                std::cerr << "L=" << L << std::endl;

                //______________________________________________________________
                //
                // construct most precise Cstar, checking that the linear
                // problem is consistent
                //______________________________________________________________
                matrix<double> P2(Nc,Nc);
                tao::mmul_rtrn(P2,P,P);
                dP2 = ideterminant(P2);
                if(Fabs(dP2)<=0)
                {
                    throw exception("boot.%s: singular system of constraints", *name);
                }
                iadjoint(aP2,P2);

                tao::mul(U,aP2,L);
                tao::mul_trn(Xstar,P,U);
                tao::divby(dP2,Xstar);
                std::cerr << "Xstar=" << Xstar << std::endl;

                //______________________________________________________________
                //
                //
                // construct one orthonormal subspace
                //
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

                //______________________________________________________________
                //
                //
                // construct an initial point
                //
                //______________________________________________________________

                // start at cstart
                tao::set(Xorg,Xstar);

                // initial balance along Q
                balance(cs.active);
                std::cerr << "Xpls=" << Xorg << std::endl;

                // and a final equilibrium close to a constrained concentration
                if(!cs.normalize(Xorg,t,true))
                {
                    throw exception("boot.%s: unable to normalize guess concentration",*name);
                }
                std::cerr << "Xorg=" << Xorg << std::endl;

                //______________________________________________________________
                //
                //
                // motion along valid concentration
                //
                //______________________________________________________________
                matrix<double>  tP(P,YOCTO_MATRIX_TRANSPOSE);
                double R0 = RMS(Xorg);
                std::cerr << "R0=" << R0 << std::endl;
                while(true)
                {
                    //__________________________________________________________
                    //
                    // compute the constrained increase
                    //
                    //__________________________________________________________
                    tao::mul(dL,P,Xorg);           // dL = P*Xorg
                    tao::subp(dL,L);               // dL = L-P*Xorg;
                    tao::mul(U,aP2,dL);            // U  = adjoint(P*P')*dL
                    tao::mul_and_div(dX,tP,U,dP2); // dX = P'*inv(P*P')*(L-P*Xorg)

                    double alpha = 1.0;
                    double R1    = __Control(alpha);
                    std::cerr << "Xtry=" << Xtry << " ; R1=" << R1 << "/" << R0 << std::endl;
                    if(R1>=R0)
                    {
                        triplet<double> xx = { 0, alpha, alpha };
                        triplet<double> rr = { R0, R1, R1 };
                        optimize1D<double>::run(Control,xx,rr);
                        R1 = __Control(alpha=max_of<double>(0.0,xx.b));
                    }

                    tao::set(Xorg,Xtry);
                    if(R1>=R0)
                    {
                        R0=R1;
                        break;
                    }
                    R0=R1;
                }
                


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
                // std::cerr << "Xorg=" << Xorg << std::endl;
                //std::cerr << "beta=" << beta << std::endl;
                tao::mul(V,Q,beta);
                tao::mul_trn(dX,Q,V);
                //std::cerr << "dX=" << dX << std::endl;
                double alpha = 1;
                double E1    = __Balance(alpha);
                if(E1<=0)
                {
                    //@Xtry
                    break;
                }

                triplet<double> xx = { 0, alpha, alpha };
                triplet<double> ee = { E0, E1, E1 };
                bracket<double>::expand(Balance,xx,ee);
                optimize1D<double>::run(Balance,xx,ee);
                E1 = __Balance(alpha=max_of<double>(0,xx.b));
                std::cerr << "E1=" << E1 << " / " << E0 << " (alpha=" << alpha << ")" << std::endl;
                if(E1<=0||E1>=E0)
                {
                    //@Xtry
                    break;
                }

                // new step
                E0 = E1;
                tao::set(Xorg,Xtry);
            }
            // out of the loop
            for(size_t j=M;j>0;--j)
            {
                if(active[j])
                {
                    Xorg[j] = max_of<double>(0.0,Xtry[j]);
                }
                else
                {
                    Xorg[j] = Xtry[j];
                }
            }
        }

    }

}


