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
                    xi[i] /= nu2[i];
                }
                tao::mul(dC,NuT,xi);
                
                //std::cerr << "C   =" << C    << std::endl;
                //std::cerr << "beta=" << beta << std::endl;

                
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
                std::cerr << "E1(" << alpha << ")=" << E1 << "/" << E0 << std::endl;

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
                std::cerr << "rms=" << rms << std::endl;
                if(E1>=E0 || rms<=0 )
                {
                    std::cerr << "balance.reached_minimum" << std::endl;
                    goto CHECK;
                }

                E0 = E1;
                goto CYCLE;
            }

        CHECK:
            std::cerr << "balance.check" << std::endl;
            std::cerr << "C =" << C  << std::endl;
            for(size_t i=N;i>0;--i)
            {
                std::cerr << "|_checking for '" << peqs[i]->name << "'" << std::endl;
                const array<double> &nu = Nu[i];
                double vmax = 0;
                size_t nbad = 0;
                for(size_t j=M;j>0;--j)
                {
                    if(Fabs(nu[j])>0)
                    {
                        assert(active[j]);
                        const double c = C[j];
                        const double v = Fabs(c);
                        if(c<0)
                        {
                            ++nbad;
                        }
                        dC[j] = c;
                        if(v>vmax)
                        {
                            vmax = v;
                        }
                    }
                    else
                    {
                        dC[j] = 0;
                    }
                }
                if(nbad<=0)
                {
                    std::cerr << "balance.cleared" << std::endl;
                    tao::set(C0,C,M);
                    return true;
                }
                std::cerr << "C=" << dC << std::endl;
                std::cerr << "vmax=" << vmax << std::endl;
            }

            exit(0);

        }
        

#if 0
        double equilibria:: __minCG(double alpha)
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
        
        static inline
        void __optimize(numeric<double>::function &F,
                        triplet<double>           &xx,
                        triplet<double>           &ff )
        {
            assert(xx.a<=xx.b);assert(xx.b<=xx.c);
            double delta = Fabs(xx.c-xx.a);
            while(true)
            {
                kernel::minimize(F,xx,ff);
                const double d_new = Fabs(xx.c-xx.a);
                if(d_new>=delta)
                {
                    return;
                }
                delta = d_new;
            }
        }
        
        bool equilibria:: balance2(array<double> &C0) throw()
        {
            std::cerr << "balance2" << std::endl;
            //__________________________________________________________________
            //
            // initialize: transfert and compute first E0
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
            
            if(E0<=0)
            {
                std::cerr << "balance.OK" << std::endl;
                return true;
            }
            
            //__________________________________________________________________
            //
            // initialize conjugated directions
            //__________________________________________________________________
            tao::mul(xi,Nu,beta);
            for(size_t i=N;i>0;--i)
            {
                assert(nu2[i]>0);
                xi[i] /= nu2[i];
            }
            tao::mul(gg,NuT,xi);
            tao::set(hh,gg);
           
            
        CYCLE:
            {
                //______________________________________________________________
                //
                // Loop: hh, gg and C are defined
                //______________________________________________________________
                std::cerr << "C   =" << C << std::endl;
                std::cerr << "beta=" << beta << std::endl;
                std::cerr << "gg  ="  << gg << std::endl;
                std::cerr << "hh  =" << hh << std::endl;
                
                double alpha= 1;
                double E1   = minCG(alpha);

                {
                    triplet<double> xx = { 0,  alpha, alpha };
                    triplet<double> ff = { E0, E1,    E1    };
                    bracket<double>::expand(minCG,xx,ff);
                    xx.co_sort(ff);
                    __optimize(minCG,xx,ff);
                    E1 = minCG(alpha=xx.b);
                }
                
                //______________________________________________________________
                //
                // early return ?
                //______________________________________________________________
                if(E1<=0)
                {
                    std::cerr << "balance.success" << std::endl;
                    tao::set(C0,Ctry,M);
                    return true;
                }


                //______________________________________________________________
                //
                // analyze result
                //______________________________________________________________

                if(E1>=E0)
                {
                    std::cerr << "balance.reached level 1" << std::endl;
                    for(size_t j=M;j>0;--j)
                    {
                        dC[j] = Ctry[j] - C[j];
                        C[j]  = Ctry[j];
                    }
                    goto CHECK;
                }
                
                assert(E1<E0);
                
                //______________________________________________________________
                //
                // compute new descent direction in dC
                //______________________________________________________________
                tao::mul(xi,Nu,beta);
                for(size_t i=N;i>0;--i)
                {
                    assert(nu2[i]>0);
                    xi[i] /= nu2[i];
                }
                tao::mul(dC,NuT,xi);
                
                //______________________________________________________________
                //
                // compute new descent direction in dC
                //______________________________________________________________
                std::cerr << "balance.CG" << std::endl;
                double g2 = 0;
                double dg = 0;
                for(size_t j=M;j>0;--j)
                {
                    const double g = gg[j];
                    const double d = dC[j];
                    g2  += g*g;
                    dg  += (d-g)*d;
                }
                if(g2<=numeric<double>::minimum)
                {
                    std::cerr << "balance.reached level-2" << std::endl;
                }
                const double fac = max_of<double>(dg/g2,0);
                std::cerr << "|_fac=" << fac << std::endl;
                for(size_t j=M;j>0;--j)
                {
                    hh[j] = gg[j] + fac * hh[j];
                    gg[j] = dC[j];
                }
                goto CYCLE;
            }
        CHECK:


            exit(0);
        }
#endif
    }
}

