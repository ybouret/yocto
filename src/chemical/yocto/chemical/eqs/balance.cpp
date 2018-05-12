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
                if(active[j])
                {
                    // compute trial concentration
                    const double Cj = (Ctry[j]=C[j]+alpha*dC[j]);
                    if(Cj<0)
                    {
                        const double Cj2=Cj*Cj;
                        if(Cj2>=numeric<double>::tiny)
                        {
                            // is significant
                            beta[j] = -Cj;
                            E += Cj2;
                        }
                        else
                        {
                            // not significant
                            Ctry[j] = 0;
                            beta[j] = 0;
                            assert(Fabs(beta[j])<=0);
                        }
                    }
                }
                else
                {
                    // just copy data
                    assert(Fabs(dC[j])<=0);
                    beta[j] = 0;
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

        bool equilibria:: balance(array<double> &C0) throw()
        {
            equilibrium::range rng;

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
                    const double extent = (xi[i] /= nu2[i]);
                    std::cerr << ".." << peqs[i]->name << " : xi_guess=" << extent << " : ";
                    if(extent>0)
                    {
                        peqs[i]->compute_forward(rng,C);
                        std::cerr << " fwd: " << rng << std::endl;
                    }
                    else if(extent<0)
                    {
                        peqs[i]->compute_reverse(rng,C);
                        std::cerr << " rng: " << rng << std::endl;
                    }
                    else
                    {
                        std::cerr << " nope" << std::endl;
                    }
                }
                tao::mul(dC,NuT,xi);
                std::cerr << "C   =" << C    << std::endl;
                std::cerr << "beta=" << beta << std::endl;
                std::cerr << "desc=" << dC   << std::endl;
                //exit(0);
                
                //______________________________________________________________
                //
                // line minimization
                //______________________________________________________________
                double alpha = 1;
                double E1    = callE(alpha);
                {
                    triplet<double> xx = { 0,  alpha, alpha };
                    triplet<double> ff = { E0, E1,    E1    };
                    bracket<double>::expand(callE,xx,ff);
                    xx.co_sort(ff);
                    __optimize(callE,xx,ff);
                    alpha = max_of<double>(xx.b,0); // never go back
                    E1    = __callE(alpha);
                }
                //std::cerr << "E1(" << alpha << ")=" << E1 << "/" << E0 << std::endl;

                //______________________________________________________________
                //
                // early return ?
                //______________________________________________________________
                double rms = 0;
                for(size_t j=M;j>0;--j)
                {
                    const double d  = Ctry[j] - C[j];
                    const double d2 = d*d;
                    if(d2>numeric<double>::tiny)
                    {
                        rms += d2;
                        dC[j] = d;
                    }
                    else
                    {
                        dC[j] = 0;
                    }
                    C[j] = Ctry[j];
                }

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

                rms = sqrt(rms/M);
                //std::cerr << "rms=" << rms << std::endl;
                if(E1>=E0
                   //|| rms<=0
                   )
                {
                    std::cerr << "balance.reached_minimum@" << E1 << std::endl;
                    goto CHECK;
                }

                E0 = E1;
                goto CYCLE;
            }

        CHECK:
            std::cerr << "balance.check" << std::endl;
            std::cerr << "C =" << C  << std::endl;
            for(size_t j=M;j>0;--j)
            {
                const double Cj = C[j];
                if(!active[j] || Cj>=0) continue; // good...
                std::cerr << "..checking C[" << j << "]=" << Cj << std::endl;
                bool can_be_zero = true;
                bool was_met     = false;

                // loop over equilibria
                for(size_t i=N;i>0;--i)
                {
                    const array<double> &nu   = Nu[i];
                    const double        coeff = Fabs(nu[j]);
                    std::cerr << "|_in '" << peqs[i]->name << "'" << std::endl;
                    if(coeff<=0)
                    {
                        std::cerr << "|_not involved" << std::endl;
                        continue;
                    }
                    was_met = true;
                    const double dxi = -Cj/coeff;
                    std::cerr << "dxi=" << dxi << std::endl;
                    for(size_t k=M;k>0;--k)
                    {
                        if(!active[k]) continue;
                        const double Ck = C[k];
                        if(Ck>=0)
                        {
                            
                        }
                    }
                }

            }
            exit(0);

        }
        
        bool equilibria:: balance2(array<double> &C0) throw()
        {
            matrix<double> U(M,M);
            tao::mmul(U,NuT,Nu);
            std::cerr << "Q=" << U << std::endl;
            matrix<double> V(M,M);
            vector<double> w(M,0);
            if(!svd<double>::build(U,w,V))
            {
                std::cerr << "no svd" << std::endl;
                exit(1);
            }
            std::cerr << "w=" << w << std::endl;
            svd<double>::set_image_dimension(w,N);
            std::cerr << "w=" << w << std::endl;
            std::cerr << "U=" << U << std::endl;
            std::cerr << "V=" << V << std::endl;

            for(size_t j=M;j>0;--j)
            {
                beta[j] = 0;
                const double Cj = (C[j]=C0[j]);
                if(active[j]&&Cj<0)
                {
                    beta[j] = -Cj;
                }
            }
            std::cerr << "C="    << C    << std::endl;
            std::cerr << "beta=" << beta << std::endl;
            svd<double>::solve(U,w,V,beta,dC);
            std::cerr << "lam=" << dC << std::endl;
            exit(0);

        }


        
    }
}

