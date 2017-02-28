#ifndef YOCTO_ALCHEMY_INTEGRATOR_INCLUDED
#define YOCTO_ALCHEMY_INTEGRATOR_INCLUDED 1

#include "yocto/alchemy/equilibria.hpp"
#include "yocto/math/ode/explicit/driver-ck.hpp"

namespace yocto
{

    namespace alchemy
    {

        typedef math::ode::driverCK<double>::type odeint;

        class integrator : public odeint
        {
        public:
            explicit integrator( const equilibria::pointer &user_eqs, const double user_eps);
            virtual ~integrator() throw();

            



        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(integrator);
            equilibria::pointer pEqs;
        };


    }

}


#endif
