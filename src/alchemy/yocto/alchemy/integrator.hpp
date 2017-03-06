#ifndef YOCTO_ALCHEMY_INTEGRATOR_INCLUDED
#define YOCTO_ALCHEMY_INTEGRATOR_INCLUDED 1

#include "yocto/alchemy/equilibria.hpp"
#include "yocto/math/ode/explicit/driver-ck.hpp"

namespace yocto
{

    namespace alchemy
    {

        // type of integrator
        typedef math::ode::driverCK<double>::type odeint;

        typedef math::ode::Field<double>::Equation ode_type;
        typedef math::ode::Field<double>::Callback ode_proc;

        class integrator : public odeint
        {
        public:
            explicit integrator(const equilibria::pointer &user_eqs,
                                const double               user_eps);
            virtual ~integrator() throw();

            

            void project(array<double>       &sigma,
                        const array<double> &y,
                        const double t);

            void call_normalize(array<double> &y, const double t);


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(integrator);
            equilibria::pointer pEqs;
            ode_proc            normalize;
        };


    }

}


#endif
