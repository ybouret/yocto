#include "yocto/chemical/equilibria.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/adjoint.hpp"

namespace yocto
{
    using namespace math;
    
    namespace chemical
    {
        
        bool equilibria:: balance() throw()
        {
            equilibrium::range fwd,rev;

#if 1
            {
                size_t ii=1;
                for(iterator i=begin();i!=end();++i,++ii)
                {
                    const equilibrium &eq = **i;
                    eq.check_ranges(fwd,rev,C);
                    std::cerr << eq.name;
                    spaces_for(eq.name,std::cerr);
                    std::cerr << ": fwd=" << fwd << ", rev=" << rev << std::endl;
                }
            }
#endif

            size_t nbad  = 0;
            for(size_t j=M;j>0;--j)
            {
                beta[j] = 0;
                const double Cj = C[j];
                if(active[j]&&(Cj<0))
                {
                    beta[j] = -Cj;
                    ++nbad;
                }
            }
            if(nbad<=0)
            {
                std::cerr << "balanced" << std::endl;
                return true;
            }
            else
            {
                std::cerr << "C   =" << C << std::endl;
                std::cerr << "beta=" << beta << std::endl;
                tao::mul_and_div(xi,AdjNu,beta,Det);
                std::cerr << "xi=" << xi << std::endl;

                // Analyzing extent...
                size_t i=1;
                for(iterator ii=begin();ii!=end();++i,++ii)
                {
                    const equilibrium &eq     = **ii;
                    const int          status = eq.check_ranges(fwd,rev,C);
                    double            &extent = xi[i];
                    switch(status)
                    {
                        case equilibrium::has_fwd:
                            break;

                        case equilibrium::has_rev:
                            break;

                        case equilibrium::has_both:
                            break;

                        default:
                            assert(equilibrium::has_none==status);
                            break;
                    }
                }

                tao::mul(dC,NuT,xi);
                std::cerr << "dC=" << dC << std::endl;
                return false;
            }


#if 0
            size_t nbad  = 0;
            for(size_t j=M;j>0;--j)
            {
                beta[j] = 0;
                const double Cj = C[j];
                if(active[j]&&(Cj<0))
                {
                    beta[j] = -Cj;
                    ++nbad;
                }
            }
            
            if(nbad<=0)
            {
                std::cerr << "balanced" << std::endl;
                return true;
            }
            else
            {
                //______________________________________________________________
                //
                // compute the projection of beta on Nu range
                //______________________________________________________________
                std::cerr << "nbad=" << nbad << std::endl;
                std::cerr << "C   =" << C    << std::endl;
                std::cerr << "beta=" << beta << std::endl;
                tao::mul_and_div(dC,Psi,beta,Det);
                std::cerr << "dC=" << dC << std::endl;
                
                //______________________________________________________________
                //
                // analyse step
                //______________________________________________________________
                double decrease_coeff = 0;
                size_t decrease_index = 0;
                
                double increase_coeff = 0;
                size_t increase_index = 0;
                
                for(size_t j=M;j>0;--j)
                {
                    if(!active[j]) continue;
                    const double c = C[j];
                    const double d = dC[j];
                    
                    if(d<0)
                    {
                        //------------------------------------------------------
                        // decrease concentration: should never get negative!
                        //------------------------------------------------------
                        if(c<=0)
                        {
                            std::cerr << "..blocked by #" << j << std::endl;
                            return false;
                        }
                        else
                        {
                            assert(c>0);
                            const double coeff = c/(-d);
                            std::cerr << "dec : " << j << " : " << coeff << std::endl;
                            if( (decrease_index<=0) || (coeff<decrease_coeff) )
                            {
                                decrease_index = j;
                                decrease_coeff = coeff;
                            }
                        }
                    }
                    else if(d>0)
                    {
                        //------------------------------------------------------
                        // increase concentration: no more than reaching zero
                        //------------------------------------------------------
                        if(c<0)
                        {
                            const double coeff = (-c)/d;
                            std::cerr << "inc : " << j << " : " << coeff << std::endl;
                            if( (increase_index<=0) || (coeff>increase_coeff) )
                            {
                                increase_index = j;
                                increase_coeff = coeff;
                            }
                        }
                    }
                    
                }
                
               
                std::cerr << "decrease_index = " << decrease_index << " : " << decrease_coeff << std::endl;
                std::cerr << "increase_index = " << increase_index << " : " << increase_coeff << std::endl;
                
                if(increase_index<=0)
                {
                    std::cerr << "no increasing concentration" << std::endl;
                    return false;
                }
                else
                {
                    return false;
                }
            }
#endif
        }
        
    }

}

