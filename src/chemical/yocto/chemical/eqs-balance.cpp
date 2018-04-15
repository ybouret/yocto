#include "yocto/chemical/equilibria.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/opt/minimize.hpp"
#include "yocto/math/opt/bracket.hpp"

namespace yocto
{
    using namespace math;
    
    namespace chemical
    {

#if 0
        double equilibria:: callE(double alpha)
        {
            tao::setprobe(Ctry, C, alpha, dC);
            double ans = 0.0;
            for(size_t j=M;j>0;--j)
            {
                const double Cj = Ctry[j];
                if(active[j] && (Cj<0) )
                {
                    ans += Cj * Cj;
                }
            }
            return ans*0.5;
        }
#endif
        
        void   equilibria:: stay_positive() throw()
        {
            for(size_t j=M;j>0;--j)
            {
                if(active[j])
                {
                    C[j] = max_of<double>(C[j],0);
                }
            }
        }

        bool equilibria:: balance( )
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
                const double Cj = C[j];
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
            std::cerr << "E0=" << E0/2 << std::endl;
            if(E0<=0)
            {
                // everything is fine
                stay_positive();
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
                    const double c = C[j];
                    const double d = dC[j];
                    //std::cerr << c << " -> " << d << std::endl;
                    if(d<0)
                    {
                        //______________________________________________________
                        //
                        // find limiting species with decreasing concentration
                        //______________________________________________________
                        const double atmp = c/(-d);
                        std::cerr << "\tamax=" << atmp << "@" << j << std::endl;
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
                                std::cerr << "\tamin=" << atmp << "@" << j << std::endl;
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
                if(jmax<=0&&jmin<=0)
                {
                    throw exception("singular balancing!");
                }

                if(jmax>0)
                {
                    // decreasing limitation
                    if(amax<=0)
                    {
                        std::cerr << "balancing is blocked" << std::endl;
                        return false;
                    }
                    else
                    {
                        // update
                        std::cerr << "maximum advance" << std::endl;
                        tao::muladd(C,amax,dC);
                        C[jmax] = 0;
                        std::cerr << "C=" << C << std::endl;
                        goto BALANCE;
                    }
                }
                else
                {
                    // no decreasing limitation
                    assert(jmin>0);
                    std::cerr << "minimum advance" << std::endl;
                    tao::muladd(C,amin,dC);
                    C[jmin] = 0;
                    std::cerr << "C=" << C << std::endl;
                    goto BALANCE;
                }

                exit(1);
            }
        }

    }

}

