#include "yocto/chemical/equilibria.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/opt/minimize.hpp"
#include "yocto/math/opt/bracket.hpp"

namespace yocto
{
    using namespace math;
    
    namespace chemical
    {

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

        bool equilibria:: balance( )
        {
            //__________________________________________________________________
            //
            // Initial Check
            //__________________________________________________________________
            double E0 = 0;
            for(size_t j=M;j>0;--j)
            {
                const double Cj = C[j];
                if(active[j] && (Cj<0) )
                {
                    E0     +=  Cj*Cj;
                    beta[j] = -Cj;
                }
            }
            E0 *= 0.5;
            std::cerr << "E0=" << E0 << std::endl;
            if(E0<=0)
            {
                return true;
            }

            //__________________________________________________________________
            //
            // Following gradient
            //__________________________________________________________________
        BALANCE:
            {
                tao::mul(dC,nu2,beta);
                std::cerr << "beta=" << beta << std::endl;
                std::cerr << "dC  =" << dC << std::endl;
                triplet<double> XX = { 0,1,-1 };
                triplet<double> EE = { E0,E(XX.b),-1};
                const double    X1 = optimize1D<double>::forward_run(E, XX, EE, 0);
                const double    E1 = EE.b;
                std::cerr << "E1=" << E1 << "@+" << X1 << std::endl;
                tao::set(C,Ctry);
                if(E1<=0)
                {
                    return true;
                }
                if(E1<E0)
                {
                    E0 = E1;
                    for(size_t j=M;j>0;--j)
                    {
                        const double Cj = C[j];
                        if(active[j] && (Cj<0) )
                        {
                            beta[j] = -Cj;
                        }
                    }
                    goto BALANCE;
                }
                else
                {

                    return false;
                }
            }
        }

    }

}

