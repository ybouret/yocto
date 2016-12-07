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
    chemlib->add("AH",0);
    chemlib->add("A-",-1);

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

    equilibrium eq2( "acid",  chemlib, 1e-4  );
    eq2.add("AH",-1);
    eq2.add("A-", 1);
    eq2.add("H+", 1);

    std::cerr << "eq1: " << eq1 << std::endl;
    std::cerr << "eq2: " << eq2 << std::endl;

    double K1 = 0;
    double G1 = eq1.computeGamma(C, 0.0, K1);
    std::cerr << "G1=" << G1 << ", K1=" << K1 << std::endl;

    vector<double> Phi1( chemlib->size(), -1000 );
    eq1.computeGradient(Phi1, C, K1);
    std::cerr << "Phi1=" << Phi1 << std::endl;

    double K2 = 0;
    double G2 = eq2.computeGamma(C, 0.0, K2);
    std::cerr << "G2=" << G2 << ", K2=" << K2 << std::endl;
    vector<double> Phi2( chemlib->size(), -1000 );
    eq2.computeGradient(Phi2, C, K2);
    std::cerr << "Phi2=" << Phi2 << std::endl;

}
YOCTO_UNIT_TEST_DONE()
