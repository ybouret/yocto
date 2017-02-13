#include "yocto/alchemy/equilibria.hpp"
#include "yocto/code/utils.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/opt/bracket.hpp"
#include "yocto/math/opt/minimize.hpp"

namespace yocto
{
    using namespace math;

    namespace alchemy
    {

        void equilibria:: compute(XiLimits &limits, const array<int> &nu) const throw()
        {

            limits.reset();
            //__________________________________________________________________
            //
            // Loop over species
            //__________________________________________________________________
            for(size_t j=M;j>0;--j)
            {
                const int nu_j = nu[j];
                if(nu_j>0)
                {
                    const double Xj = max_of<double>(C[j]/nu_j,0.0);
                    std::cerr << "\tproduct,   nu=" << nu_j <<", [" << (*pLib)(j)->name << "]=" << C[j] << ", xi=" << Xj << std::endl;
                    //__________________________________________________________
                    //
                    // this is a product, so there is a reverse limitation
                    //__________________________________________________________
                    XiLimit &rev = limits.reverse;
                    if(!rev.exists)
                    {
                        rev.exists = true;
                        rev.index  = j;
                        rev.value  = Xj;
                    }
                    else
                    {
                        if(Xj<rev.value)
                        {
                            rev.index = j;
                            rev.value = Xj;
                        }
                    }
                    continue;
                }

                if(nu_j<0)
                {
                    const double Xj = max_of<double>(C[j]/(-nu_j),0.0);
                    std::cerr << "\treactant, -nu=" << -nu_j <<", [" << (*pLib)(j)->name << "]=" << C[j] << ", xi=" << Xj << std::endl;
                    //__________________________________________________________
                    //
                    // this is a reactant, so there is a forward limitation
                    //__________________________________________________________
                    XiLimit &fwd = limits.forward;
                    if(!fwd.exists)
                    {
                        fwd.exists = true;
                        fwd.index  = j;
                        fwd.value  = Xj;
                    }
                    else
                    {
                        if(Xj<fwd.value)
                        {
                            fwd.index = j;
                            fwd.value = Xj;
                        }
                    }
                    continue;
                }
            }

            if(limits.forward.exists)
            {
                std::cerr << "\t\tforward limit: " << limits.forward.value << ", reached for " << (*pLib)(limits.forward.index)->name << std::endl;
            }
            else
            {
                std::cerr << "\t\tno forward limit" << std::endl;
            }

            if(limits.reverse.exists)
            {
                std::cerr << "\t\treverse limit: " << limits.reverse.value << ", reached for " << (*pLib)(limits.reverse.index)->name << std::endl;
            }
            else
            {
                std::cerr << "\t\tno reverse limit" << std::endl;
            }


        }


        bool equilibria:: try_balance(const size_t      j,
                                      const XiLimits   &limits,
                                      const array<int> &nu) throw()
        {
            const int         nu_j = nu[j];
            if(!nu_j)
                return false; // this reaction doesn't handle the species

            assert(active[j]);assert(beta[j]<0);
            const double Cj = beta[j];

            if(nu_j>0)
            {
                const double xx = (-Cj)/nu_j;
                std::cerr << "\t" << (*pLib)(j)->name << " is a product, need a possible forward step of " << xx << std::endl;
                const XiLimit &fwd = limits.forward;
                if(!fwd.exists || (fwd.exists&&xx<=fwd.value) )
                {
                    std::cerr << "\tusing fwd, nu=" << nu << std::endl;
                    for(size_t k=M;k>0;--k)
                    {
                        if(active[k])
                        {
                            C[k] += xx * nu[k];
                        }
                    }
                    if(fwd.exists&&(fwd.index==j))
                    {
                        C[j] = 0;
                    }
                    return true;
                }
            }

            if(nu_j>0)
            {
                const double xx = (-Cj)/(-nu_j);
                std::cerr << "\t" << (*pLib)(j)->name << " is a reactant, need a possible reverse step of " << xx << std::endl;
                const XiLimit &rev = limits.reverse;
                if(!rev.exists || (rev.exists&&xx<=rev.value) )
                {
                    std::cerr << "\tusing rev, nu=" << nu << std::endl;
                    for(size_t k=M;k>0;--k)
                    {
                        if(active[k])
                        {
                            C[k] -= xx * nu[k];
                        }
                    }
                    if(rev.exists&&(rev.index==j))
                    {
                        C[j] = 0;
                    }
                    return true;
                }
            }


            return false;
        }


        void equilibria:: balance()
        {
            std::cerr << std::endl << "Balancing..." << std::endl;

        TRY_BALANCE:
            pLib->display(std::cerr,C);

            //__________________________________________________________________
            //
            // dectect bad concentrations
            //__________________________________________________________________
            size_t nbad = 0;
            for(size_t j=M;j>0;--j)
            {
                const double Cj = C[j];
                beta[j] = 0;
                if(active[j]&&Cj<0)
                {
                    ++nbad;
                    beta[j] = Cj;
                }
            }

            if(nbad<=0)
            {
                std::cerr << "Cgood=" << C << std::endl;
                return;
            }

            //__________________________________________________________________
            //
            // Loop on reactions to try to solve the problem
            //__________________________________________________________________
            for(size_t i=N;i>0;--i)
            {
                //______________________________________________________________
                //
                // extract the local topology and compute the limits according
                // to the current set of concentrations
                //______________________________________________________________
                std::cerr << "querying '" << (*this)(i)->name << "'" << std::endl;

                const array<int> &nu   = iNu[i];
                XiLimits          limits;
                compute(limits,nu);

                //______________________________________________________________
                //
                // try to use this reaction to balance invalid concentrations
                //______________________________________________________________
                for(size_t j=M;j>0;--j)
                {
                    const double Cj = beta[j];
                    if(Cj>=0.0) continue;
                    if(try_balance(j,limits,nu))
                    {
                        // at least one concentration has changed...
                        goto TRY_BALANCE;
                    }
                }
            }
            throw imported::exception("alchemy.equilibria.balance","no possible balance");
        }

        double equilibria:: call_E(const double alpha)
        {
            tao::setprobe(dC, C, alpha, h);
            double ans = 0;
            for(size_t i=M;i>0;--i)
            {
                const double tmp = dC[i];
                if(active[i] && tmp < 0 )
                {
                    ans += tmp * tmp;
                }
            }
            return ans;
        }

        void equilibria:: call_g(array<double> &grad, const array<double> &Ctmp)
        {
            for(size_t i=M;i>0;--i)
            {
                const double CC = Ctmp[i];
                if(active[i] && CC < 0 )
                {
                    beta[i] = -CC;
                }
            }
            tao::mul(grad, Nu2, beta);
        }




        void equilibria:: balance2()
        {
            static const char fn[] = "equilibria.balance: ";

            std::cerr << "-- Initialize Balancing" << std::endl;
            std::cerr << "C=" << C << std::endl;
            // initialize objective function
            double E0 = E(0.0);
            std::cerr << "E0=" << E0 << std::endl;
            if(E0<=0)
            {
                return; // everything is fine
            }

            // initialize gradient
            call_g(g,dC);
            std::cerr << "beta=" << beta << std::endl;
            std::cerr << "g   =" << g    << std::endl;

            double g2 = tao::norm_sq(g);
            if(g2<=0)
            {
                throw exception("%sinvalid initial gradient",fn);
            }
            tao::set(h,g);
            std::cerr << "h=" << h << std::endl;

            while(true)
            {
                std::cerr << "\tE0=" << E0 << std::endl;
                // find minimum in h direction
                triplet<double> xx = { 0.0, 1.0,     0.0 };
                triplet<double> EE = { E0,  E(xx.b), 0.0 };
                std::cerr << "-- Initial" << std::endl;
                std::cerr << "xx=" << xx << std::endl;
                std::cerr << "EE=" << EE << std::endl;
                std::cerr << "h=" << h << std::endl;


                std::cerr << "-- Bracketing" << std::endl;
                math::bracket<double>::expand(E,xx,EE);

                std::cerr << "xx=" << xx << std::endl;
                std::cerr << "EE=" << EE << std::endl;

                std::cerr << "-- Optimize" << std::endl;

                math::optimize1D<double>::run(E, xx, EE, 0.0);
                std::cerr << "xx=" << xx << std::endl;
                std::cerr << "EE=" << EE << std::endl;


                // advance point
                const double E1 = E(xx.b);
                tao::set(C,dC);
                std::cerr << "C=" << C << std::endl;


                std::cerr << "E0=" << E0 <<  " -> " << E1 << std::endl;
                if(E1>=E0)
                {
                    throw exception("%sunable to balance concentrations",fn);
                }
                E0 = E1;
                if(E0<=0)
                {
                    break; // success
                }

                call_g(b,dC);
                const double b2 = tao::norm_sq(b);
                if(b2<=0)
                {
                    throw exception("%sinvalid current gradient",fn);
                }
                double dgg = 0;
                for(size_t i=M;i>0;--i)
                {
                    dgg += (b[i]-g[i])*b[i];
                }
                const double gam = dgg/g2;

                g2 = b2;
                tao::set(g,b);
                tao::setprobe(h,g,gam,h);

            }

            assert(E0<=0);


        }
        
    }
    
}

