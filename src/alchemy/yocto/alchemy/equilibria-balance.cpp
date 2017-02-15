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

        void equilibria:: compute_descent(array<double>       &descent,
                                          const array<double> &Ctmp)
        {
            for(size_t i=M;i>0;--i)
            {
                const double CC = Ctmp[i];
                if(active[i] && CC < 0 )
                {
                    beta[i] = -CC;
                }
            }
            tao::mul(descent, Nu2, beta);
        }




        void equilibria:: balance()
        {
            static const char fn[] = "equilibria.balance: ";

            std::cerr << "-- Initialize Balancing" << std::endl;
            std::cerr << "C=" << C << std::endl;

            //__________________________________________________________________
            //
            // initialize objective function
            //__________________________________________________________________
            double E0 = E(0.0);
            std::cerr << "E0ini=" << E0 << std::endl;
            if(E0<=0)
            {
                return; // everything is fine
            }

            //__________________________________________________________________
            //
            // initialize descent directions
            //__________________________________________________________________
            compute_descent(g,dC);
            std::cerr << "beta=" << beta << std::endl;
            std::cerr << "g   =" << g    << std::endl;

            double g2 = tao::norm_sq(g);
            if(g2<=0)
            {
                throw exception("%sinvalid initial gradient",fn);
            }
            tao::set(h,g);
            std::cerr << "h=" << h << std::endl;


            //__________________________________________________________________
            //
            // conjugated gradient loop
            //__________________________________________________________________
            while(true)
            {
                std::cerr << "\tE0=" << E0 << std::endl;

                //______________________________________________________________
                //
                // find minimum in h direction
                //______________________________________________________________
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

                //______________________________________________________________
                //
                // advance point by recomputing dC at optimal point
                // and setting new C to it
                //______________________________________________________________
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

                //______________________________________________________________
                //
                // update h by algorithm
                //______________________________________________________________
                compute_descent(b,dC);
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

                //______________________________________________________________
                //
                // TODO: ensure h is in Im(Nu2)
                //______________________________________________________________

            }

            assert(E0<=0);
            //______________________________________________________________
            //
            // avoid numerical glitches
            //______________________________________________________________
            for(size_t j=M;j>0;--j)
            {
                if(active[j])
                {
                    C[j] = max_of<double>(C[j],0.0);
                }
            }

            pLib->display(std::cerr,C);

        }
        
    }
    
}

