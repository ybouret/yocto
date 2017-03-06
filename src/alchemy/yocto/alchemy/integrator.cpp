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

        void integrator:: project(array<double>        &rate,
                                  const array<double>  &C,
                                  const double          t)
        {
            pEqs->project(rate,C,t);
        }
        
        void integrator::forward(ode_type      &sigma,
                                 array<double> &y,
                                 const double   t1,
                                 const double   t2,
                                 double        &tscale)
        {
            odeint &self = *this;
            self(sigma, y, t1, t2, tscale, &normalize);
        }
    }

}
