#include "yocto/chemical/equilibria.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/adjoint.hpp"

namespace yocto
{
    using namespace math;
    
    namespace chemical
    {
        
#if 0
        bool equilibria:: balance( array<double> &C0 ) throw()
        {
            //__________________________________________________________________
            //
            //
            // Initial Check: compute E0 and possible beta
            //
            //__________________________________________________________________
        BALANCE:
            double E0 = 0;
            for(size_t j=M;j>0;--j)
            {
                const double Cj = C0[j];
                if(active[j]&&(Cj<0))
                {
                    E0     +=  Cj*Cj;
                    beta[j] = -Cj;
                }
                else
                {
                    beta[j] = 0;
                }

            }
            //std::cerr << "E0=" << E0/2 << std::endl;
            if(E0<=0)
            {
                // everything is fine
                std::cerr << "\t<balanced>" << std::endl;
                //stay_positive();
                return true;
            }

            //__________________________________________________________________
            //
            //
            // computing increases and check them
            //
            //__________________________________________________________________
            {
                //______________________________________________________________
                //
                // compute the concentration descent direction
                //______________________________________________________________

                tao::mul(dC,nu2,beta);
                std::cerr << "beta=" << beta << std::endl;
                std::cerr << "dC  =" << dC << std::endl;
                //______________________________________________________________
                //
                // forward optimize
                //______________________________________________________________
                double amax = 0; //!< limitation to decrease concentration
                double jmax = 0; //!< the limiting index

                double amin = 0; //!< limitation to increase negative conc
                double jmin = 0; //!< the limiting index

                for(size_t j=M;j>0;--j)
                {
                    const double c = C0[j];
                    const double d = dC[j];
                    //std::cerr << c << " -> " << d << std::endl;
                    if(d<0)
                    {
                        //______________________________________________________
                        //
                        // find limiting species with decreasing concentration
                        //______________________________________________________
                        const double atmp = c/(-d);
                        //std::cerr << "\tamax=" << atmp << "@" << j << std::endl;
                        if(jmax<=0)
                        {
                            amax = atmp;
                            jmax = j;
                        }
                        else
                        {
                            if(atmp<amax)
                            {
                                amax = atmp;
                                jmax = j;
                            }
                        }
                    }
                    else
                    {
                        //______________________________________________________
                        //
                        // find minimal possible species with increasing conc
                        //______________________________________________________
                        assert(d>=0);
                        if(d>0)
                        {
                            if(c<0)
                            {
                                const double atmp = (-c)/d;
                                //std::cerr << "\tamin=" << atmp << "@" << j << std::endl;
                                if(jmin<=0)
                                {
                                    amin = atmp;
                                    jmin = j;
                                }
                                else
                                {
                                    if(atmp>amin)
                                    {
                                        amin = atmp;
                                        jmin = j;
                                    }
                                }
                            }
                        }
                    }
                }
                std::cerr << "amax=" << amax << "@" << jmax << std::endl;
                std::cerr << "amin=" << amin << "@" << jmin << std::endl;
                if( (jmax<=0) && (jmin<=0) )
                {
                    return false;
                }

                if(jmax>0)
                {
                    //__________________________________________________________
                    //
                    // decreasing limitation
                    //__________________________________________________________
                    if(amax<=0)
                    {
                        std::cerr << "balancing is blocked" << std::endl;
                        return false;
                    }
                    else
                    {
                        //______________________________________________________
                        //
                        // update
                        //______________________________________________________
                        std::cerr << "maximum advance" << std::endl;
                        tao::muladd(C0,amax,dC);
                        C0[jmax] = 0;
                        std::cerr << "C0=" << C0 << std::endl;
                        goto BALANCE;
                    }
                }
                else
                {
                    //__________________________________________________________
                    //
                    // no decreasing limitation
                    //__________________________________________________________
                    assert(jmin>0);
                    std::cerr << "minimum advance" << std::endl;
                    tao::muladd(C0,amin,dC);
                    C0[jmin] = 0;
                    std::cerr << "C0=" << C0 << std::endl;
                    goto BALANCE;
                }

            }
        }
#endif
        
        bool equilibria:: balance() throw()
        {
          
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
                
                // analyse step
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
                            if((coeff<=1.0) &&
                               ( (decrease_index<=0) || (coeff<decrease_coeff) )
                               )
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
                            if((coeff<=1.0) &&
                               ( (increase_index<=0) || (coeff>increase_coeff) )
                               )
                            {
                                increase_index = j;
                                increase_coeff = coeff;
                            }
                        }
                    }
                    
                }
                
                
                std::cerr << "decrease_index = " << decrease_index << " : " << decrease_coeff << std::endl;
                std::cerr << "increase_index = " << increase_index << " : " << increase_coeff << std::endl;


                return false;
            }
            
        }
        
    }

}

