#include "yocto/alchemy/integrator.hpp"
#include "yocto/math/core/tao.hpp"

namespace yocto
{
    namespace alchemy
    {
        integrator:: ~integrator() throw()
        {
        }

        integrator:: integrator(const equilibria::pointer &user_eqs, const double user_eps) :
        odeint(user_eps),
        pEqs(user_eqs),
        normalize(this, &integrator::call_normalize )
        {

        }


        void integrator::call_normalize(array<double> &y, const double t)
        {
            assert(y.size()>=pEqs->pLib->size());
            pEqs->normalize(y,t);
        }

        void integrator:: project(array<double>       &sigma,
                                  const array<double> &y,
                                  const double         t)
        {
            
        }
        
        
    }
}
