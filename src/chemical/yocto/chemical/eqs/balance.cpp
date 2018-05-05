#include "yocto/chemical/equilibria.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/adjoint.hpp"
#include "yocto/math/core/svd.hpp"

namespace yocto
{
    using namespace math;
    
    namespace chemical
    {


        void equilibrium:: compute_ranges( range &fwd, range &rev, const array<double> &C) const throw()
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

            rev.clear();

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
#if 0
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
#endif

            double min_bad_C = 0;
            size_t min_bad_j = 0;
            size_t nbad      = 0;
            for(size_t j=M;j>0;--j)
            {
                beta[j] = 0;
                const double Cj = C0[j];
                if(active[j]&&(Cj<0))
                {
                    ++nbad;
                    std::cerr << "bad C[" << j << "]=" << Cj << std::endl;
                    const double bad_C = -Cj;
                    if(min_bad_j<=0 || bad_C < min_bad_C )
                    {
                        min_bad_j = j;
                        min_bad_C = bad_C;
                    }
                }
            }
            if(nbad<=0)
            {
                return true;
            }
            else
            {
                assert(min_bad_j);
                assert(min_bad_C>0);
                std::cerr << "Trying to resorb " << min_bad_C << " @" << min_bad_j << std::endl;
                beta[min_bad_j] = min_bad_C;
                const array<double> &v = NuT[min_bad_j];
                std::cerr << "v=" << v << std::endl;
                return false;
            }


            if(nbad<=0)
            {
                return true;
            }
            else
            {
                std::cerr << "beta=" << beta << std::endl;
                std::cerr << "C   =" << C << std::endl;
                equilibrium::range fwd,rev;
                size_t i=1;
                for(iterator ii=begin();i<=N;++ii,++i)
                {
                    equilibrium &eq = **ii;
                    eq.compute_ranges(fwd,rev,C);
                    std::cerr << eq.name;
                    spaces_for(eq.name,std::cerr) << " : fwd=" << fwd << ", rev=" << rev << std::endl;
                }
            }

            return false;
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

