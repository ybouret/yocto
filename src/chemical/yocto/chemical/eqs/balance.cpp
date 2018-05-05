#include "yocto/chemical/equilibria.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/adjoint.hpp"
#include "yocto/math/core/svd.hpp"

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
                    const int    nu = a->nu; assert(nu<0);
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




        bool equilibria:: balance(array<double> &C0) throw()
        {
            equilibrium::range fwd,rev;

            size_t nbad      = 0;
            for(size_t j=M;j>0;--j)
            {
                beta[j] = 0;
                const double Cj = C0[j];
                if(active[j]&&(Cj<0))
                {
                    ++nbad;
                    beta[j] = -Cj;
                }
            }
            if(nbad<=0)
            {
                return true;
            }
            else
            {
                
            }

#if 0
            if(nbad<=0)
            {
                return true;
            }
            else
            {
                //______________________________________________________________
                //
                // computing the desired extent to resorb the smallest excess
                //______________________________________________________________
                std::cerr << "beta=" << beta << std::endl;
                tao::ld(xi,0);
                for(size_t j=1;j<=M;++j)
                {
                    if(beta[j]>0)
                    {
                        assert(active[j]);
                        const array<double> &Vj  = NuT[j];
                        const double         V2  = tao::norm_sq(Vj); assert(V2>0);
                        const double         lam = beta[j]/V2;
                        std::cerr << "beta[" << j << "]=" << beta[j] << "\t => " << lam << '*' << Vj << std::endl;
                    }
                }
                return false;

                //______________________________________________________________
                //
                // clipping
                //______________________________________________________________
                size_t i=1;
                for(iterator ii=begin();i<=N;++ii,++i)
                {
                    equilibrium &eq = **ii;
                    double      &extent = xi[i];
                    std::cerr << eq.name; spaces_for(eq.name,std::cerr) << ": ";
                    if(extent>0)
                    {
                        eq.compute_forward(fwd,C);
                        std::cerr << "fwd=" << fwd << std::endl;
                    }
                    else
                    {
                        if(extent<0)
                        {
                            eq.compute_reverse(rev,C);
                            std::cerr << "rev=" << rev << std::endl;
                        }
                        else
                        {
                            std::cerr << "nope" << std::endl;
                            continue; // do nothing
                        }
                    }
                }


                return false;
            }
#endif


        }


#if 0
        bool equilibria:: balance(array<double> &C0) throw()
        {

            assert(C0.size()>=M);
            
        BALANCE:
            //__________________________________________________________________
            //
            // construct the bad vector
            //__________________________________________________________________
            size_t nbad = 0;
            for(size_t j=M;j>0;--j)
            {
                beta[j] = 0;
                const double Cj = C0[j];
                if(active[j]&&(Cj<0))
                {
                    beta[j] = -Cj;
                    ++nbad;
                }
            }
            if(nbad<=0)
            {
                return true;
            }
            else
            {
                //______________________________________________________________
                //
                // try to resorb it
                //______________________________________________________________
                tao::mul(xi,Nu,beta); //!< projection on equation
                tao::mul(dC,NuT,xi);  //!< global decrease direction

                //______________________________________________________________
                //
                // analyze the step
                //______________________________________________________________
                size_t decr_index = 0;
                double decr_value = 0;
                size_t incr_index = 0;
                double incr_value = 0;
                for(size_t j=M;j>0;--j)
                {
                    //__________________________________________________________
                    //
                    // gather status
                    //__________________________________________________________
                    if(!active[j]) { assert(Fabs(dC[j])<=0); continue; }
                    const double c = C0[j];
                    const double d = dC[j];

                    if(d>0)
                    {
                        //______________________________________________________
                        //
                        // increasing a concentration
                        //______________________________________________________
                        if(c<0)
                        {
                            // this is one of the bad!
                            // compute the smallest step
                            const double tmp = (-c)/d;
                            if( (incr_index<=0) || (tmp<incr_value) )
                            {
                                incr_value = tmp;
                                incr_index = j;
                            }
                        }
                        // else nothing to do...
                    }
                    else if(d<0)
                    {
                        //______________________________________________________
                        //
                        // decreasing a concentration: don't let
                        // them become invalid!
                        //______________________________________________________
                        if(c<=0)
                        {
                            //this is one of the bad: too bad!!!
                            return false;
                        }
                        else
                        {
                            const double tmp = c/(-d);
                            if( (decr_index<=0) || (tmp<decr_value) )
                            {
                                decr_index = j;
                                decr_value = tmp;
                            }
                        }
                    }
                }

                if(incr_index<=0)
                {
                    // shoudn't happen
                    return false;
                }
                assert(incr_value>0);
                double fac = incr_value;
                size_t idx = incr_index;
                if(decr_index)
                {
                    if(decr_value<incr_value)
                    {
                        // takes precedence
                        fac = decr_value;
                        idx = decr_index;
                    }
                }

                // updating carefully
                for(size_t j=M;j>0;--j)
                {
                    if(!active[j])
                    {
                        assert(Fabs(dC[j])<=0);
                        continue;
                    }

                    const bool is_bad = (C0[j]<0);
                    C0[j] += fac * dC[j];
                    if(is_bad)
                    {
                         if(C0[j]>=0) C0[j]=0;
                    }
                    else
                    {
                        if(C0[j]<=0) C0[j]=0;
                    }
                }
                C0[idx] = 0;
                goto BALANCE;
            }

        }
#endif

        
    }

}

