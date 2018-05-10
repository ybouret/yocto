#include "yocto/chemical/equilibria.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/adjoint.hpp"
#include "yocto/math/core/svd.hpp"
#include "yocto/math/core/symdiag.hpp"
#include "yocto/sort/quick.hpp"
#include "yocto/math/opt/bracket.hpp"
#include "yocto/math/opt/minimize.hpp"
#include "yocto/math/types.hxx"

namespace yocto
{
    using namespace math;
    
    namespace chemical
    {

        void equilibrium::compute_extents( range &fwd, range &rev, const array<double> &C) const throw()
        {
            compute_forward(fwd,C);
            compute_reverse(rev,C);
        }

        void equilibrium:: compute_forward( range &fwd, const array<double> &C) const throw()
        {
            fwd.clear();

            for(const actor *a=reactants.head;a;a=a->next)
            {
                const size_t id = a->sp->indx;
                const double c  = C[id];
                if(c<0)
                {
                    // blocked
                    fwd.exists=true;
                    fwd.xi    =0;
                    fwd.id    =0;
                    fwd.no    =id;
                }
                else
                {
                    const int    nu = a->nu;   assert(nu<0);
                    const double xi = c/(-nu); assert(xi>=0);
                    if( (!fwd.exists) || (xi<fwd.xi) )
                    {
                        fwd.exists = true;
                        fwd.xi     = xi;
                        fwd.id     = id;
                    }
                }
            }
        }

        void equilibrium:: compute_reverse( range &rev, const array<double> &C) const throw()
        {

            for(const actor *a=products.head;a;a=a->next)
            {
                const size_t id = a->sp->indx;
                const double c  = C[id];
                if(c<0)
                {
                    // blocked
                    rev.exists=true;
                    rev.xi    =0;
                    rev.id    =0;
                    rev.no    =id;
                }
                else
                {
                    const int    nu = a->nu; assert(nu>0);
                    const double xi = -c/(nu); assert(xi<=0);
                    if( (!rev.exists) || (xi>rev.xi) )
                    {
                        rev.exists = true;
                        rev.xi     = xi;
                        rev.id     = id;
                    }
                }
            }
        }

        double equilibria:: __callE(double alpha)
        {
            double E = 0;
            for(size_t j=M;j>0;--j)
            {
                const double Cj = (Ctry[j]=C[j]+alpha*dC[j]);
                beta[j] = 0;
                if(active[j]&&Cj<0)
                {
                    beta[j] = -Cj;
                    E      += square_of(Cj);
                }
            }
            return E;
        }

        static inline
        void __optimizeE(numeric<double>::function &EE,
                         triplet<double>           &xx,
                         triplet<double>           &ff )
        {
            assert(xx.a<=xx.b);assert(xx.b<=xx.c);
            double delta = Fabs(xx.c-xx.a);
            while(true)
            {
                kernel::minimize(EE,xx,ff);
                const double d_new = Fabs(xx.c-xx.a);
                if(d_new>=delta)
                {
                    return;
                }
                delta = d_new;
            }
        }

        bool equilibria:: balance(array<double> &C0) throw()
        {

            //__________________________________________________________________
            //
            // initialize: transfert and compute first E
            //__________________________________________________________________
            double E0   = 0;
            for(size_t j=M;j>0;--j)
            {
                beta[j] = 0;
                const double Cj = (C[j]=C0[j]);
                if(active[j]&&(Cj<0))
                {
                    beta[j] = -Cj;
                    E0 += square_of(Cj);
                }
            }

            //__________________________________________________________________
            //
            // at this point, E0, beta and C are set
            //__________________________________________________________________
            if(E0<=0)
            {
                std::cerr << "balance.OK" << std::endl;
                return true;
            }

        BALANCE_CYCLE:
            {
                // at this point, E0, beta and C are set

                std::cerr << "balance.E0=" << E0 << std::endl;

                // compute direction on xi space
                tao::mul(xi,Nu,beta);
                for(size_t i=N;i>0;--i)
                {
                    xi[i] /= nu2[i];
                }
                // compute direction on C space
                tao::mul(dC,NuT,xi);

                std::cerr << "C   =" << C    << std::endl;
                std::cerr << "beta=" << beta << std::endl;
                std::cerr << "dC  =" << dC   << std::endl;

                double alpha = 1;
                double E1    = callE(alpha);
                std::cerr << "E1=" << E1 << "/" << E0 << std::endl;
                triplet<double> xx = { 0,  alpha, alpha };
                triplet<double> ff = { E0, E1,    E1    };
                if(E1<E0)
                {
                    std::cerr << "balance.bracket.expand" << std::endl;
                    bracket<double>::expand(callE,xx,ff);
                }
                else
                {
                    assert(E1>=E0);
                    std::cerr << "balance.bracket.inside" << std::endl;
                    bracket<double>::inside(callE,xx,ff);
                }
                __optimizeE(callE,xx,ff);
                alpha = xx.b;
                E1    = callE(alpha);
                tao::setvec(dC,C,Ctry);
                tao::set(C,Ctry);
                const double dC2 = tao::norm_sq(dC);

                std::cerr << "E1(" << alpha << ")=" << E1 << std::endl;
                if(E1<=0)
                {
                    std::cerr << "balance.perfect!" << std::endl;
                    tao::set(C0,C);
                    return true;
                }
                if(E1>=E0||dC2<=0)
                {
                    std::cerr << "balance.reached" << std::endl;
                    std::cerr << "C0 =" << C0 << std::endl;
                    std::cerr << "C  =" << C << std::endl;
                    std::cerr << "dC =" << dC << std::endl;
                    std::cerr << "dC2=" << dC2 << std::endl;
                    exit(0);
                }
                else
                {
                    E0 = E1;
                    goto BALANCE_CYCLE;
                }


                return false;
            }

        }

        double equilibria:: __computeE(double alpha)
        {
            double E = 0;
            for(size_t j=M;j>0;--j)
            {
                const double Cj = (Ctry[j]=C[j]+alpha*hh[j]);
                beta[j] = 0;
                if(active[j]&&Cj<0)
                {
                    beta[j] = -Cj;
                    E      += square_of(Cj);
                }
            }
            return E;
        }

        bool equilibria:: balance2(array<double> &C0) throw()
        {
            //__________________________________________________________________
            //
            //
            // initialize: transfer, compute E0, beta
            //
            //__________________________________________________________________

            // build E0 and beta at the same time
            double E0   = 0;
            for(size_t j=M;j>0;--j)
            {
                beta[j] = 0;
                const double Cj = (C[j]=C0[j]);
                if(active[j]&&(Cj<0))
                {
                    beta[j] = -Cj;
                    E0 += square_of(Cj);
                }
            }

            if(E0<=0)
            {
                std::cerr << "balanced.OK" << std::endl;
                return true;
            }

            std::cerr << "E0    =" << E0 << std::endl;
            std::cerr << "C0    =" << C << std::endl;
            std::cerr << "beta0 =" << beta << std::endl;

            // compute the initial descent directions
            tao::mul(xi,Nu,beta);
            for(size_t i=N;i>0;--i)
            {
                xi[i] /= nu2[i];
            }
            tao::mul(gg,NuT,xi); // principal  direction
            tao::set(hh,gg);     // conjugated direction
            std::cerr << "g0=" << gg << std::endl;
            std::cerr << "h0=" << hh << std::endl;

        BALANCE_STEP:
            {
                assert(E0>0);
                //______________________________________________________________
                //
                //
                // at this point, C, E0, gg and hh are set:
                // compute the new concentration by line minimization along hh
                //
                //______________________________________________________________
                std::cerr << std::endl;
                std::cerr << "balance.E0=" << E0 << std::endl;

                double alpha = 1;
                double E1    = computeE(alpha);
                {
                    triplet<double> xx = { 0,  alpha, alpha };
                    triplet<double> ff = { E0, E1,    E1    };

                    if(E1<E0)
                    {
                        std::cerr <<  "balance.expand" << std::endl;
                        bracket<double>::expand(computeE,xx,ff);
                    }
                    else
                    {
                        assert(E1>=E0);
                        std::cerr << "balance.inside" << std::endl;
                        bracket<double>::inside(computeE,xx,ff);
                    }
                    alpha = xx.b;
                    E1    = __computeE(alpha);
                }
                tao::set(C,Ctry);
                std::cerr << "E1(" << alpha << ")=" << E1 << "/" << E0 << std::endl;

                //______________________________________________________________
                //
                //
                // check balance status
                //
                //______________________________________________________________
                if(E1<=0)
                {
                    std::cerr << "balance.found" << std::endl;
                    for(size_t j=M;j>0;--j)
                    {
                        C0[j] = C[j];
                    }
                    return true;
                }

                bool converged = false;
                if(E1>=E0)
                {
                    std::cerr << "balance.reached level-1" << std::endl;
                    converged = true;
                }

                //______________________________________________________________
                //
                //
                // let's compute the new principal descent direction in dC
                //
                //______________________________________________________________
                tao::mul(xi,Nu,beta);
                for(size_t i=N;i>0;--i)
                {
                    xi[i] /= nu2[i];
                }
                tao::mul(dC,NuT,xi);
                if(converged)
                {
                    goto BALANCE_CONVERGED; // no need to go further
                }

                //______________________________________________________________
                //
                //
                // compute the conjugated gradient from new direction dC,
                // and old direction gg
                //
                //______________________________________________________________
                double g2 = 0;
                double dg = 0;
                for(size_t j=M;j>0;--j)
                {
                    g2  += gg[j] * gg[j];
                    dg  -= (gg[j]-dC[j])*dC[j];
                }
                std::cerr << "g2=" << g2 << std::endl;
                if(g2<=numeric<double>::minimum)
                {
                    std::cerr << "balanced.reached level-2" << std::endl;
                    goto BALANCE_CONVERGED;
                }

                const double fac = dg/g2;
                for(size_t j=M;j>0;--j)
                {
                    gg[j] = dC[j];
                    hh[j] = gg[j] + fac * hh[j];
                }
                std::cerr << "C   =" << C    << std::endl;
                std::cerr << "beta=" << beta << std::endl;
                std::cerr << "gg  =" << gg   << std::endl;
                std::cerr << "hh  =" << hh   << std::endl;
                E0 = E1 ;

                goto BALANCE_STEP;
            }

        BALANCE_CONVERGED:
            std::cerr << "balance.converged: " << std::endl;
            for(size_t j=1;j<=M;++j)
            {
                std::cerr << "C0[" << j << "]=" << C0[j] << " -> " << C[j] << " : dC=" << dC[j] << std::endl;
            }
            exit(0);
            return false;

        }


    }
}

