#include "yocto/chemical/equilibria.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/adjoint.hpp"

namespace yocto
{
    using namespace math;
    
    namespace chemical
    {



        
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


        
    }

}

