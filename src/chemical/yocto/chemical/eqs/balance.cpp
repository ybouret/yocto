#include "yocto/chemical/equilibria.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/adjoint.hpp"
#include "yocto/math/core/svd.hpp"
#include "yocto/math/core/symdiag.hpp"
#include "yocto/sort/quick.hpp"
#include "yocto/math/opt/bracket.hpp"
#include "yocto/math/opt/minimize.hpp"

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

            // at this point, E0, beta and C are set
            if(E0<=0)
            {
                std::cerr << "balanced" << std::endl;
                return true;
            }
            else
            {
            BALANCE_CYCLE:
                std::cerr << "E0=" << E0 << std::endl;

                // compute on xi space
                tao::mul(xi,Nu,beta);
                for(size_t i=N;i>0;--i)
                {
                    xi[i] /= nu2[i];
                }
                // compute on C space
                tao::mul(dC,NuT,xi);

                std::cerr << "C   =" << C    << std::endl;
                std::cerr << "beta=" << beta << std::endl;
                std::cerr << "xi  =" << xi   << std::endl;
                std::cerr << "dC  =" << dC   << std::endl;

                double alpha = 1;
                double E1    = callE(alpha);
                std::cerr << "E1=" << E1 << "/" << E0 << std::endl;
                triplet<double> xx = { 0,  alpha, alpha };
                triplet<double> ff = { E0, E1,    E1    };
                if(E1<E0)
                {
                    std::cerr << "bracket.expand" << std::endl;
                    bracket<double>::expand(callE,xx,ff);
                }
                else
                {
                    assert(E1>=E0);
                    std::cerr << "bracket.inside" << std::endl;
                    bracket<double>::inside(callE,xx,ff);
                }
                std::cerr << "alpha=" << xx << std::endl;
                std::cerr << "E    =" << ff  << std::endl;
                __optimizeE(callE,xx,ff);
                alpha = xx.b;
                E1    = callE(alpha);
                tao::set(C,Ctry);
                std::cerr << "C1=" << C << std::endl;
                std::cerr << "E1(" << alpha << ")=" << E1 << std::endl;
                if(E1<=0)
                {
                    std::cerr << "perfectly balanced..." << std::endl;
                    return true;
                }
                if(E1>=E0)
                {
                    std::cerr << "Reached limit..." << std::endl;
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



    }
}

