#include "yocto/chemical/integrator.hpp"
#include "yocto/code/utils.hpp"
namespace yocto
{
    namespace chemical
    {
        integrator:: ~integrator() throw()
        {
        }

        integrator:: integrator(const double user_ftol) :
        yocto::chemical::ode_driver(user_ftol),
        pSystem(0),
        pScheme(0),
        damped( this, & integrator:: __damped )
        {
        }

        void  integrator:: operator()(ode_type      &scheme,
                                      equilibria    &eqs,
                                      array<double> &C0,
                                      const double   t0,
                                      const double   t1)
        {
            pScheme = &scheme;
            pSystem = &eqs;
            double      dt   = t1-t0;
            ode_driver &self = *this;
            self.operator()(damped,C0,t0,t1,dt,&eqs.project);
        }

        void integrator:: __damped(array<double> &dCdt, double t, const array<double> &C)
        {
            assert(pSystem);
            assert(pScheme);

            equilibria &cs = *pSystem;

            // compute initial guess
            (*pScheme)(dCdt,t,C);

            // chemical damping
            if(!cs.damp(dCdt,C,t,true))
            {
                throw exception("chemical.integrator: singular system @t=%g", t);
            }
            
        }


    }

}

