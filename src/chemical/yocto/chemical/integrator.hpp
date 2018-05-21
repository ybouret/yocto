#ifndef YOCTO_CHEMICAL_INTEGRATOR_INCLUDED
#define YOCTO_CHEMICAL_INTEGRATOR_INCLUDED 1

#include "yocto/chemical/equilibria.hpp"
#include "yocto/math/ode/explicit/driver-ck.hpp"

namespace yocto
{
    namespace chemical
    {

        typedef math::ode::driverCK<double>::type ode_driver;
        typedef ode_driver::equation              ode_type;

        //! must be started with the right dimension before user
        class integrator : public ode_driver
        {
        public:
            explicit integrator( const double user_ftol );
            virtual ~integrator() throw();


            //! assuming that
            void operator()(ode_type      &scheme,
                            equilibria    &eqs,
                            array<double> &C0,
                            const double   t0,
                            const double   t1);
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(integrator);
            equilibria *pSystem;
            ode_type   *pScheme;
            ode_type    damped;
            void __damped(array<double> &dCdt, double t, const array<double> &C);

        };

    }
}

#endif


