#include "yocto/alchemy/equilibria.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/lu.hpp"


namespace yocto
{
    using namespace math;

    namespace alchemy
    {

        void equilibria:: project(array<double>       &rate,
                                  const array<double> &C0,
                                  const double t)
        {
            computeChi(C0,t);
            tao::mul(xi,Phi,rate);
            LU<double>::solve(Chi,xi);
            tao::mul_sub_trn(rate,Nu,xi);
        }
    }

}

