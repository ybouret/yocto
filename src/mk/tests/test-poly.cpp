#include "yocto/utest/run.hpp"
#include "yocto/math/core/polynomial-utils.hpp"
#include "yocto/ios/ocstream.hpp"

using namespace yocto;
using namespace math;

YOCTO_UNIT_TEST_IMPL(poly)
{
    typedef polynomial<double> Poly;
    
    Poly P;
    
    std::cerr << P << std::endl;
    P.add(2,2);
    std::cerr << P << std::endl;
    P.add(1,3);
    std::cerr << P << std::endl;
    P.add(0,1);
    std::cerr << P << std::endl;
    
    P.clear();
    
    P.add(0,2);
    P.add(1,3);
    P.add(2,4);
    std::cerr << "P=" << P << std::endl;
    
    Poly Q;
    Q.add(3,2);
    Q.add(6,8);
    
    std::cerr << "Q=" << Q << std::endl;
    
    P = Poly::mul(P, Q);
    std::cerr << "PQ=" << P << std::endl;
    std::cerr << "degree=" << P.degree() << std::endl;
    while(P.degree()>0)
    {
        P = Poly::derivative(P);
        std::cerr << "dP=" << P << std::endl;
    }
    P.clear();
    
    P.add(0,2);
    P.add(1,3);
    P.add(2,4);
    std::cerr << "P=" << P << std::endl;
    P = Poly::primitive(P);
    std::cerr << "iP=" << P << std::endl;
    
    P.clear();
    Q.clear();
    
    P.add(0,1);
    P.add(1,2);
    P.add(2,3);
    
    Q = P;
    
    std::cerr << "P=" << P << std::endl;
    std::cerr << "Q=" << Q << std::endl;
    Poly::derivative(P,Q);
    std::cerr << P << "/" << Q << std::endl;
    
    {
        const double t1  = 1.0;
        const double t2  = 2.0;
        const double f1  = sin(t1);
        const double df1 = cos(t1);
        const double f2  = sin(t2);
        const double df2 = cos(t2);
        __poly::gap(P,t1, f1, df1, t2, f2, df2);
        const size_t NE = 10;
        const size_t NP = 100;
        {
            ios::wcstream fp("poly.dat");
            for(size_t i=0;i<=NE;++i)
            {
                const double t = (i*t1)/NE;
                fp("%g %g\n",t,sin(t));
            }

            for(size_t i=0;i<=NP;++i)
            {
                const double t = t1 +(i*(t2-t1))/NP;
                fp("%g %g\n",t,P(t-t1));
            }

            for(size_t i=0;i<=NE;++i)
            {
                const double t = t2 + (i*(3.14-t2))/NE;
                fp("%g %g\n",t,sin(t));
            }
        }
    }
    
}
YOCTO_UNIT_TEST_DONE()
