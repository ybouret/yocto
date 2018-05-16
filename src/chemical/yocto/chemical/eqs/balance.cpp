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



        
        double equilibria:: __Balance(double alpha)
        {
            double E = 0;
            for(size_t j=M;j>0;--j)
            {
                beta[j] = 0;
                if(active[j])
                {
                    // compute trial concentration
                    const double Cj = (Ctry[j]=C[j]+alpha*dC[j]);
                    const double Cj2 = Cj*Cj;
                    if(Cj2>=numeric<double>::tiny)
                    {
                        // significant
                        if(Cj<0)
                        {
                            beta[j] = -Cj;
                            E      += Cj2;
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
            return E;
        }
        
        
        
        

        bool equilibria:: balance(array<double> &C0,const bool normal) throw()
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
                return true;
            }

            //__________________________________________________________________
            //
            // End of initialization: set C to controlled Ctry
            //__________________________________________________________________
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
                    //std::cerr << "balance.success" << std::endl;
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
                    std::cerr << "balance.reached_minimum@" << E1 << std::endl;
                    goto CHECK;
                }

                E0 = E1;
                goto CYCLE;
            }

        CHECK:
            std::cerr << "balance.check" << std::endl;
            std::cerr << "C=" << C << std::endl;
            if(normal)
            {
                //__________________________________________________________________
                //
                //
                // check invalid values to see if balanced is kept by zeroing them
                // in all involved equilibria
                //
                //__________________________________________________________________
                for(size_t j=M;j>0;--j)
                {
                    const double Cj = C[j];
                    if(!active[j] || Cj>=0 ) continue;
                    assert(active[j]&&Cj<0);
                    bool was_met = false; //!< because all concentration may be negative for one equilibrium => not tested! => Bad
                    bool is_zero = true;  //!< is can be zeroed

                    //______________________________________________________________
                    //
                    // loop over all reactions
                    //______________________________________________________________
                    for(size_t i=N;i>0;--i)
                    {
                        const array<double> &nu_i   = Nu[i];
                        const double         nu_ij  = nu_i[j]; if(Fabs(nu_ij)<=0) continue;
                        const double         extent = Fabs(Cj/nu_ij); //!< the absolute extent

                        //__________________________________________________________
                        //
                        // loop over other components
                        //__________________________________________________________
                        for(size_t k=M;k>0;--k)
                        {
                            const double nu_ik = nu_i[k];  if(Fabs(nu_ik)<=0) continue; assert(active[k]);
                            const double Ck    = C[k];     if(Ck<0)           continue; assert(k!=j);
                            const double delta = Fabs(nu_ik*extent);
                            was_met = true;
                            if( delta > numeric<double>::ftol * Ck )
                            {
                                is_zero = false;
                                break;
                            }
                        }
                    }
                    //std::cerr << "was_met=" << was_met << std::endl;
                    //std::cerr << "is_zero=" << is_zero << std::endl;
                    if(was_met&&is_zero)
                    {
                        C[j] = 0;
                    }
                    else
                    {
                        //std::cerr << "balance.blocked" << std::endl;
                        return false;
                    }
                }
            }
            else
            {
                for(size_t j=M;j>0;--j)
                {
                    if(active[j]&&C[j]<0)
                    {
                        C[j] = 0;
                    }
                }
            }
            tao::set(C0,C,M);
            return true;
        }
        

    }
}

