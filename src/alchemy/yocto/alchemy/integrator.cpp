#include "yocto/alchemy/integrator.hpp"

namespace yocto
{
    namespace alchemy
    {
        integrator:: ~integrator() throw()
        {
        }

        integrator:: integrator(const equilibria::pointer &eqs) :
        odeint(1e-4),
        pEqs(eqs)
        {

        }
    }
}
