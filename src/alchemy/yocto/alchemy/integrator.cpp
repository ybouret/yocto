#include "yocto/alchemy/integrator.hpp"

namespace yocto
{
    namespace alchemy
    {
        integrator:: ~integrator() throw()
        {
        }

        integrator:: integrator(const equilibria::pointer &user_eqs, const double user_eps) :
        odeint(user_eps),
        pEqs(user_eqs)
        {

        }
    }
}
