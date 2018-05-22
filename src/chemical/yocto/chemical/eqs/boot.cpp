
#include "yocto/chemical/boot.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/adjoint.hpp"
#include "yocto/math/core/svd.hpp"
#include "yocto/math/opt/minimize.hpp"
#include "yocto/math/opt/bracket.hpp"
#include "yocto/math/core/lu.hpp"
#include "yocto/math/types.hxx"
#include "yocto/ios/ocstream.hpp"
#include "yocto/sort/quick.hpp"

namespace yocto
{
    using namespace math;
    
    namespace chemical
    {



        double boot:: Error( const array<double> &XX ) throw()
        {
            tao::mul(dL,P,XX);
            double mae = 0;
            for(size_t k=Nc;k>0;--k)
            {
                const double dl = dL[k] - L[k];
                mae += sqrt( (dl*dl)/p2[k] );
            }
            return mae/Nc;
        }


        double boot:: __Control(double alpha)
        {
            tao::mulset(beta,alpha,dX);
            tao::set(Xtry,Xorg);
            if(!eqs->deliver(Xtry,beta,0,false))
            {
                throw exception("boot:%s: unable to perform control!", *name);
            }
            return Error(Xtry);
        }


        void  boot:: guess(array<double>    &C0,
                           equilibria       &cs,
                           const double      t)
        {
            static const double threshold = numeric<double>::ftol;
            clear();
            try
            {
                std::cerr << "boot <" << name << ">" << std::endl;

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
                Q.    make(N,M);
                Xorg. make(M);
                Xtry. make(M);
                beta. make(M);
                errc. make(M);
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
                
                //______________________________________________________________
                //
                // construct most precise Xstar, checking that the linear
                // problem is consistent
                //______________________________________________________________
                std::cerr << "\t...computing starting point" << std::endl;
                matrix<double> P2(Nc,Nc);
                tao::mmul_rtrn(P2,P,P);
                dP2 = ideterminant(P2);
                if(Fabs(dP2)<=0)
                {
                    throw exception("boot.%s: singular system of constraints", *name);
                }
                iadjoint(aP2,P2);

                tao::mul(U,aP2,L);
                tao::mul_trn(Xorg,P,U);
                tao::divby(dP2,Xorg);

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
                //std::cerr << "Q=" << Q << std::endl;

                //______________________________________________________________
                //
                //
                // Look for a balanced Xorg
                //
                //______________________________________________________________
                std::cerr << "\t...balancing" << std::endl;
                balance();
                double R0 = Error(Xorg);

                if(R0>threshold)
                {
                    throw exception("boot.%s: unable to balance initial constraints",*name);
                }

                //______________________________________________________________
                //
                //
                // and an initial equilibrium close to a constrained concentration
                //
                //______________________________________________________________
                std::cerr << "\t...finding initial equilibrium" << std::endl;
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
                std::cerr << "\t...iterating" << std::endl;
                matrix<double>  tP(P,YOCTO_MATRIX_TRANSPOSE);
                R0 = Error(Xorg);
                while(R0>0)
                {
                    //__________________________________________________________
                    //
                    // compute the constrained increase
                    //__________________________________________________________
                    tao::mul(dL,P,Xorg);           // dL = P*Xorg
                    tao::subp(dL,L);               // dL = L-P*Xorg;
                    tao::mul(U,aP2,dL);            // U  = adjoint(P*P')*dL
                    tao::mul_and_div(dX,tP,U,dP2); // dX = P'*inv(P*P')*(L-P*Xorg)

                    //__________________________________________________________
                    //
                    // deliver it
                    //__________________________________________________________
                    double alpha = 1.0;
                    double R1    = __Control(alpha);
                    std::cerr << "Xtry=" << Xtry << "; R1=" << R1 << "/R0=" << R0 << std::endl;
                    if(R1<R0)
                    {
                        std::cerr << "OK" << std::endl;
                        tao::set(Xorg,Xtry);
                        R0 = R1 ;
                        continue;
                    }
                    else
                    {
                        std::cerr << "Backtrack" << std::endl;
                        assert(R1>=R0);
                        triplet<double> xx = { 0,  alpha, alpha };
                        triplet<double> rr = { R0, R1,    R1    };
                        bracket<double>::inside(Control,xx,rr);
                        optimize1D<double>::run(Control,xx,rr);
                        R1 = __Control(alpha=xx.b);
                        tao::set(Xorg,Xtry);
                        if(R1>=R0)
                        {
                            R0=R1;
                            break;
                        }
                        else
                        {
                            R0 = R1 ;
                            continue;
                        }
                    }


                }
                std::cerr << "...done" << std::endl;
                tao::set(C0,Xorg,M);
            }
            catch(...)
            {
                clear(); throw;
            }
            clear();
        }

        double boot:: __Balance(double alpha) throw()
        {
            size_t nbad = 0;
            for(size_t j=M;j>0;--j)
            {
                beta[j] = 0;
                const double Xj = (Xtry[j] = Xorg[j] + alpha * dX[j]);
                const double X2 = Xj*Xj;
                if(X2<=numeric<double>::tiny)
                {
                    Xtry[j] = 0; // not significant;
                }
                else
                {
                    // significant: X2>0
                    if(eqs->active[j] && Xj<0)
                    {
                        beta[j] = -Xj;
                        errc[++nbad] = X2;
                    }
                }
            }
            if(nbad>0)
            {
                double E = 0;
                quicksort(&errc[1],nbad,__compare_decreasing<double>);
                for(size_t k=nbad;k>0;--k)
                {
                    E += errc[k];
                }
                return E;
            }
            else
            {
                return 0;
            }
        }

        void boot:: balance()
        {
            //__________________________________________________________________
            //
            // compute initial value and beta from Xorg
            //__________________________________________________________________
            double E0 = __Balance(0.0);
            if(E0<=0)
            {
                return;
            }
            
            while(true)
            {

                //______________________________________________________________
                //
                // Corg, beta and E0 are computed: compute the search dX
                //______________________________________________________________
                tao::mul(V,Q,beta);
                tao::mul_trn(dX,Q,V);
                double alpha = 1;
                double E1    = __Balance(alpha);
                if(E1<=0)
                {
                    //__________________________________________________________
                    //
                    //@Xtry: early break!
                    //__________________________________________________________
                    break;
                }

                {
                    triplet<double> xx = { 0, alpha, alpha };
                    triplet<double> ee = { E0, E1, E1 };
                    bracket<double>::expand(Balance,xx,ee);
                    optimize1D<double>::run(Balance,xx,ee);
                    E1 = __Balance(alpha=max_of<double>(0,xx.b));
                }
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
                if(eqs->active[j])
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


