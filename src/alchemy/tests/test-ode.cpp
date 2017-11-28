#include "yocto/alchemy/boot.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/alchemy/integrator.hpp"
#include "yocto/ios/ocstream.hpp"

using namespace yocto;
using namespace alchemy;


namespace
{
    static inline double Kv(const double t)
    {
        return 1e-4*(2.0 + cos(t));
    }


    class Kinetic
    {
    public:
        Kinetic()
        {
        }

        ~Kinetic() throw()
        {
        }

        void Sigma( array<double> &dCdt, const double t, const array<double> &C )
        {
            const size_t M = C.size();
            for(size_t i=M;i>0;--i)
            {
                dCdt[i] = 0;
            }
        }

    private:
        YOCTO_DISABLE_COPY_AND_ASSIGN(Kinetic);
    };
}


YOCTO_UNIT_TEST_IMPL(ode)
{



    library::pointer chemlib( new library() );

    chemlib->add("H+",1);
    chemlib->add("HO-",-1);


    chemlib->add("Na+", 1);
    chemlib->add("Cl-",-1);

    chemlib->add("AH",0);
    chemlib->add("A-",-1);

    equilibria::pointer chemsys( new equilibria(chemlib) );


    {
        equilibrium &eq = chemsys->add("water",1e-14);
        eq.add("H+", 1);
        eq.add("HO-",1);
    }



    {
        equilibrium_constant Kval = cfunctor(Kv);
        equilibrium &eq = chemsys->add("acid",Kval);
        eq.add("H+",1);
        eq.add("A-",1);
        eq.add("AH",-1);
    }

    chemsys->compile();
    
    boot loader(chemlib);


    loader.electroneutrality();
    loader.conserve("Na+", 0);
    loader.conserve("Cl-", 0);
    loader.conserve("AH", "A-", 1e-4);

    vector<double> C(chemlib->size());
    loader.solution(C,chemsys);

    Kinetic    scheme;
    ode_type   sigma( &scheme, & Kinetic::Sigma );
    
    integrator chemint(chemsys,1e-7);

    chemint.start(chemlib->size());

    double tscale = 1e-3;
    double t      = 0.0;
    double dt     = 0.1;
    {
        ios::wcstream fp("ph.dat");
        fp("%g %g\n", t,chemlib->pH(C));
    }

    while(t<=15)
    {
        const double t_next = t + dt;
        chemint.forward(sigma,C,t,t_next,tscale);

        t=t_next;
        ios::acstream fp("ph.dat");
        fp("%g %g\n", t,chemlib->pH(C));
    }

 


}
YOCTO_UNIT_TEST_DONE()

