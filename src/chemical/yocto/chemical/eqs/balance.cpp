#include "yocto/chemical/equilibria.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/svd.hpp"
#include "yocto/math/opt/bracket.hpp"
#include "yocto/math/opt/minimize.hpp"
#include "yocto/math/types.hxx"
#include "yocto/sort/quick.hpp"

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



        
        double equilibria:: __Balance(double alpha)
        {
            static const double threshold = numeric<double>::tiny;
            size_t nbad = 0;
            for(size_t j=M;j>0;--j)
            {
                beta[j] = 0;
                if(active[j])
                {
                    // compute trial concentration
                    const double Cj = (Ctry[j]=C[j]+alpha*dC[j]);
                    const double Cj2 = Cj*Cj;
                    if(Cj2>=threshold)
                    {
                        // significant
                        if(Cj<0)
                        {
                            beta[j]      = -Cj;
                            errc[++nbad] = Cj2;
                        }
                        // else nothing to do
                    }
                    else
                    {
                        // not significant, positive of negative, set to zero
                        Ctry[j] = 0;
                    }

                }
                else
                {
                    // just copy data
                    assert(Fabs(dC[j])<=0);
                    Ctry[j] = C[j];
                }
            }
            
            if(nbad)
            {
                quicksort(&errc[1],nbad,__compare_decreasing<double>);
                double E = 0;
                for(size_t k=nbad;k>0;--k)
                {
                    E+=errc[k];
                }
                return E;
            }
            else
            {
                return 0;
            }
        }
        
        
        
        

        bool equilibria:: balance(array<double> &C0,bool *changed) throw()
        {
            //__________________________________________________________________
            //
            //
            // initialize: transfert and compute first E
            //
            //__________________________________________________________________
            for(size_t j=M;j>0;--j)
            {
                C[j]  = C0[j];
            }
            double E0 = __Balance(0.0);

            //__________________________________________________________________
            //
            // at this point, E0, beta and Ctry are set
            //__________________________________________________________________
            if(E0<=0)
            {
                tao::set(C0,Ctry,M);
                if(changed) *changed=false;
                return true;
            }

            //__________________________________________________________________
            //
            // End of initialization: set C to controlled Ctry
            //__________________________________________________________________
            if(changed) *changed=true;
            tao::set(C,Ctry);
        CYCLE:
            {
                //______________________________________________________________
                //
                // at this point, E0, beta and C are set: compute
                // the descent direction
                //______________________________________________________________
                tao::mul(xi,Nu,beta);
                for(size_t i=N;i>0;--i)
                {
                    assert(nu2[i]>0);
                    (xi[i] /= nu2[i]);
                }
                tao::mul(dC,NuT,xi);

                //______________________________________________________________
                //
                // line minimization: a new Ctry is updated with beta@Ctry
                //______________________________________________________________
                double alpha = 1;
                double E1    = __Balance(alpha);
                {
                    triplet<double> xx = { 0,  alpha, alpha };
                    triplet<double> ff = { E0, E1,    E1    };
                    bracket<double>::expand(Balance,xx,ff);
                    optimize1D<double>::run(Balance,xx,ff);
                    alpha = max_of<double>(xx.b,0); // never go back
                    E1    = __Balance(alpha);
                }

                //______________________________________________________________
                //
                // early return ?
                //______________________________________________________________
                if(E1<=0)
                {
                    for(size_t j=M;j>0;--j)
                    {
                        if(active[j])
                        {
                            assert(Ctry[j]>=0);
                            C0[j] = Ctry[j];
                        }
                    }
                    return true;
                }


                //______________________________________________________________
                //
                // update result
                //______________________________________________________________
                tao::set(C,Ctry);
                if(E1>=E0)
                {
                    goto CHECK;
                }

                E0 = E1;
                goto CYCLE;
            }

        CHECK:
            double Cmax = 0;
            size_t na   = 0;
            for(size_t j=M;j>0;--j)
            {
                if(active[j])
                {
                    Cmax = max_of<double>(Cmax,C[j]);
                    ++na;
                }
            }

            const double Cnul = numeric<double>::epsilon * na * Cmax;
            
            for(size_t j=M;j>0;--j)
            {
                const double Cj = C[j];
                if(active[j]&&Cj<0)
                {
                    if(Fabs(Cj)<=Cnul)
                    {
                        C[j] = 0;
                    }
                    else
                    {
                        std::cerr << "balance.blocked" << std::endl;
                        return false;
                    }
                }
            }
            tao::set(C0,C,M);
            return true;
        }


    }
}

