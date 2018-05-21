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

        class integrator : public ode_driver
        {
        public:
            explicit integrator( const double user_ftol );
            virtual ~integrator() throw();
            
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(integrator);
        };

    }
}

#endif


