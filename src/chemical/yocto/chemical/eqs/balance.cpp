#include "yocto/chemical/equilibria.hpp"
#include "yocto/math/core/tao.hpp"
//#include "yocto/math/core/adjoint.hpp"
#include "yocto/math/core/svd.hpp"
//#include "yocto/math/core/symdiag.hpp"
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
                //______________________________________________________________
                //
                // at this point, E0, beta and C are set: compute
                // the descent direction
                //______________________________________________________________
                std::cerr << "balance.E0=" << E0 << std::endl;

                tao::mul(xi,Nu,beta);
                for(size_t i=N;i>0;--i)
                {
                    xi[i] /= nu2[i];
                }
                tao::mul(dC,NuT,xi);

                std::cerr << "C   =" << C    << std::endl;
                std::cerr << "beta=" << beta << std::endl;
                std::cerr << "xi  =" << xi   << std::endl;
                std::cerr << "dC  =" << dC   << std::endl;

                //______________________________________________________________
                //
                // line minimization
                //______________________________________________________________
                double alpha = 1;
                double E1    = callE(alpha);
                std::cerr << "E1=" << E1 << "/" << E0 << std::endl;
                {
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
                    E1    = callE(alpha); // final Ctry, and beta
                }

                //______________________________________________________________
                //
                // save the current step and update position
                //______________________________________________________________

                tao::setvec(dC,C,Ctry);
                tao::set(C,Ctry);
                const double dC2 = tao::norm_sq(dC);
                std::cerr << "E1(" << alpha << ")=" << E1 << "/" << E0 << " : |dC|^2=" << dC2 << std::endl;

                if(E1<=0)
                {
                    std::cerr << "balance.perfect!" << std::endl;
                    tao::set(C0,C,M);
                    return true;
                }
                if( (E1>=E0) || (dC2<=numeric<double>::minimum) )
                {
                    std::cerr << "balance.reached" << std::endl;
                    goto BALANCE_CHECK;

                }
                else
                {
                    E0 = E1;
                    goto BALANCE_CYCLE;
                }

            }

        BALANCE_CHECK:
            std::cerr.flush();
            for(size_t j=1;j<=M;++j)
            {
                fprintf(stderr,"C0[%2d]= %+12.8e -> %+12.8e : dC=%+12.8e\n", int(j), C0[j], C[j], dC[j]);
                //std::cerr << "C0[" << j << "]=" << C0[j] << " -> " << C[j] << " : dC=" << dC[j] << std::endl;
            }
            fflush(stderr);


            for(size_t j=M;j>0;--j)
            {
                if(!active[j]) continue;
                const double Cj = C[j];
                if(Cj<0)
                {
                    std::cerr << "Try to correct C[" << j << "]=" << Cj << std::endl;
                }
            }

            return false;

        }


        
    }
}

