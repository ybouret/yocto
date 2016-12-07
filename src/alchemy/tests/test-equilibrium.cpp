#include "yocto/alchemy/equilibrium.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/code/rand.hpp"

using namespace yocto;
using namespace alchemy;


YOCTO_UNIT_TEST_IMPL(equilibrium)
{



    library::pointer chemlib( new library() );

    chemlib->add("H+",1);
    chemlib->add("HO-",-1);

    vector<double> C( chemlib->size() + 2);
    for(size_t i=1;i<=C.size(); ++i)
    {
        C[i] = alea<double>();
    }

    chemlib->display(std::cerr,C);
    std::cerr << "C=" << C << std::endl;
    std::cerr << "osmolarity    : " << chemlib->osmolarity_of(C)     << std::endl;
    std::cerr << "molar_charge  : " << chemlib->molar_charge_of(C)   << std::endl;
    std::cerr << "ionic_strength: " << chemlib->ionic_strength_of(C) << std::endl;

    equilibrium eq1( "water", chemlib, 1e-14 );
    eq1.add("H+",1);
    eq1.add("HO-",1);
    std::cerr << "Kw=" << eq1.K(0) << std::endl;

    double Kt = 0;
    double G0 = eq1.Gamma(C, 0.0, Kt);
    std::cerr << "G0=" << G0 << ", Kt=" << Kt << std::endl;



}
YOCTO_UNIT_TEST_DONE()
