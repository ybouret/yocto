#ifndef YOCTO_ALCHEMY_INTEGRATOR_INCLUDED
#define YOCTO_ALCHEMY_INTEGRATOR_INCLUDED 1

#include "yocto/alchemy/equilibria.hpp"
#include "yocto/math/ode/explicit/driver-ck.hpp"

namespace yocto
{

    namespace alchemy
    {

        // base type of integrator
        typedef math::ode::driverCK<double>::type  odeint;

        typedef math::ode::Field<double>::Equation ode_type;
        typedef math::ode::Field<double>::Callback ode_proc;


        // will define a built in method
        /**
         must be started with a number of variables greater or equal to
         the number of species.
         */
        class integrator : public odeint
        {
        public:
            explicit integrator(const equilibria::pointer &user_eqs,
                                const double               user_eps);
            virtual ~integrator() throw();

            
            // legalise rate with chemical correction
            void project(array<double>       &rate,
                        const array<double>  &C,
                        const double          t);

            void forward(ode_type      &sigma,
                         array<double> &y,
                         const double   t1,
                         const double   t2,
                         double        &tscale);


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(integrator);
            equilibria::pointer pEqs;
            ode_proc            normalize;

            void call_normalize(array<double> &y, const double t);

        };


    }

}


#endif
