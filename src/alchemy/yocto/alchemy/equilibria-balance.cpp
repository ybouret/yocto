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




        bool equilibria:: balance() throw()
        {

            //__________________________________________________________________
            //
            // initialize objective function
            //__________________________________________________________________
            double E0 = E(0.0);
            if(E0<=0)
            {
                return true; // everything is fine
            }

            //__________________________________________________________________
            //
            // initialize descent directions
            //__________________________________________________________________
            compute_descent(g,dC);
            double g2 = tao::norm_sq(g);
            if(g2<=0)
            {
                return false;
            }
            tao::set(h,g);


            //__________________________________________________________________
            //
            // conjugated gradient loop
            //__________________________________________________________________
            while(true)
            {

                //______________________________________________________________
                //
                // find minimum in h direction
                //______________________________________________________________
                triplet<double> xx = { 0.0, 1.0,     0.0 };
                triplet<double> EE = { E0,  E(xx.b), 0.0 };

                math::bracket<double>::expand(E,xx,EE);
                math::optimize1D<double>::run(E,xx,EE, 0.0);

                //______________________________________________________________
                //
                // advance point by recomputing dC at optimal point
                // and setting new C to it
                //______________________________________________________________
                const double E1 = E(xx.b);
                tao::set(C,dC);
                if(E1>=E0)
                {
                    return false; // zero cannot be reached
                }
                E0 = E1;
                if(E0<=0)
                {
                    break;
                }

                //______________________________________________________________
                //
                // update h by algorithm
                //______________________________________________________________
                compute_descent(b,dC);
                const double b2 = tao::norm_sq(b);
                if(b2<=0)
                {
                    return false;
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
                // TODO: ensure h is in Im(Nu2) ?
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

            //pLib->display(std::cerr,C);
            return true;
        }
        
    }
    
}

