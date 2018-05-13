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
        
        
        static inline
        void __optimize(math::numeric<double>::function &F,
                        triplet<double>                 &xx,
                        triplet<double>                 &ff) throw()
        {

            double width = xx.c-xx.a; assert(width>=0);
            while(true)
            {
                kernel::minimize(F, xx, ff);
                const double new_width = xx.c-xx.a;
                assert(new_width<=width);
                if(new_width>=width)
                {
                    break;
                }
                width = new_width;
            }

        }

        static inline
        void __find_optimal(const double E0,
                            double      &alpha,
                            double      &E1,
                            numeric<double>::function &F) throw()
        {
            triplet<double> xx = { 0,  alpha, alpha };
            triplet<double> ff = { E0, E1,    E1    };
            bracket<double>::expand(F,xx,ff);
            xx.co_sort(ff);
            __optimize(F,xx,ff);
            alpha = max_of<double>(xx.b,0); // never go back
            E1    = F(alpha);
        }

        bool equilibria:: balance(array<double> &C0) throw()
        {
            //__________________________________________________________________
            //
            // initialize: transfert and compute first E
            //__________________________________________________________________
            for(size_t j=M;j>0;--j)
            {
                C[j]  = C0[j];
            }
            double E0 = __callE(0.0);

            //__________________________________________________________________
            //
            // at this point, E0, beta and Ctry are set
            //__________________________________________________________________
            if(E0<=0)
            {
                std::cerr << "balance.OK" << std::endl;
                tao::set(C0,Ctry,M);
                return true;
            }

            //__________________________________________________________________
            //
            // End of initialization
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
                double E1    = callE(alpha);
                __find_optimal(E0,alpha,E1,callE);

                //______________________________________________________________
                //
                // early return ?
                //______________________________________________________________
                if(E1<=0)
                {
                    std::cerr << "balance.success" << std::endl;
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
            for(size_t j=M;j>0;--j)
            {
                const double Cj = C[j];
                if(!active[j] || Cj>=0 ) continue;
                assert(active[j]&&Cj<0);
                std::cerr << "resorbing C[" << j << "]=" << Cj << std::endl;
                bool was_met = false;
                bool is_zero = true;
                for(size_t i=N;i>0;--i)
                {
                    const array<double> &nu_i   = Nu[i];
                    const double         nu_ij  = nu_i[j]; if(Fabs(nu_ij)<=0) continue;
                    const double         extent = Fabs(Cj/nu_ij);
                    std::cerr << "zero_xi=" << extent << std::endl;
                    for(size_t k=M;k>0;--k)
                    {
                        const double nu_ik = nu_i[k];  if(Fabs(nu_ik)<=0) continue;
                        const double Ck    = C[k];     if(Ck<0) continue;
                        const double delta = Fabs(nu_ik*extent);
                        was_met = true;
                        std::cerr << "..checked in " << peqs[i]->name << " | delta=" << delta << " / C[" << k << "]=" << Ck << std::endl;
                        if( delta > numeric<double>::ftol * Ck )
                        {
                            is_zero = false;
                            break;
                        }
                    }
                }
                std::cerr << "was_met=" << was_met << std::endl;
                std::cerr << "is_zero=" << is_zero << std::endl;
                if(was_met&&is_zero)
                {
                    C[j] = 0;
                }
                else
                {
                    std::cerr << "balance.blocked" << std::endl;
                    return false;
                }
            }
            tao::set(C0,C,M);
            return true;
        }

#if 0
        bool equilibria:: balance2(array<double> &C0) throw()
        {
            // Initialize E0 and beta
            tao::set(C,C0,M);

            // at this point, E0, C and beta are computed
            //std::cerr << "beta=" << beta << "; E0=" << E0 << std::endl;
            for(size_t i=N;i>0;--i)
            {
                std::cerr << "\t...resorbing with " << peqs[i]->name << std::endl;
                double E0 = __callE(0);
                std::cerr << "C=" << C << std::endl;
                std::cerr << "beta=" << beta << std::endl;
                double alpha = 0;
                {
                    const array<double> &nu_i = Nu[i];
                    for(size_t j=M;j>0;--j)
                    {
                        const double nu_ij = nu_i[j];
                        dC[j]   =  nu_ij;
                        alpha  +=  nu_ij*beta[j];
                    }
                    alpha /= nu2[i];
                }
                std::cerr << "alpha=" << alpha << std::endl;
                std::cerr << "dC   =" << dC << std::endl;
                double E1 = __callE(alpha);
                {
                    triplet<double> aa = { 0,  alpha, alpha };
                    triplet<double> ee = { E0, E1,    E1    };
                    bracket<double>::expand(callE,aa,ee);
                    aa.co_sort(ee);
                    std::cerr << "alpha=" << aa << std::endl;
                    std::cerr << "value=" << ee << std::endl;
                }
                std::cerr << std::endl;
            }

            exit(0);

        }
#endif

        
    }
}

