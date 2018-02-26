#include "yocto/math/fit/fit.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/sort/heap.hpp"

using namespace yocto;
using namespace math;

namespace
{
    struct Example
    {

        double compute( const double X, const array<double> &param, const Fit::Variables & )
        {
            const double a = param[1];
            const double b = param[2];
            const double c = param[3];
            return (a*b*X+c);
        }
    };
}

YOCTO_UNIT_TEST_IMPL(fit0)
{

    const size_t N     = 10 + alea.leq(50);
    const double inter = 5 * alea.symm<double>();
    const double slope = 5 * alea.symm<double>();
    vector<double> X(N);
    vector<double> Y(N);
    vector<double> Z(N);
    for(size_t i=1;i<=N;++i)
    {
        X[i] = 10 * alea.symm<double>();
        Y[i] = slope * X[i] + inter + alea.symm<double>();
    }
    co_hsort(X,Y);

    Fit::Sample<double> sample(X,Y,Z);
    sample.variables << "a" << "b" << "c";

    Example ex;
    Fit::Type<double>::Function F( &ex, & Example::compute );


    const size_t nvar = sample.variables.size();
    vector<double> aorg(nvar);
    vector<double> aerr(nvar);
    vector<bool>   used(nvar,true);

    double &a = aorg[ sample.variables["a"] ];
    double &b = aorg[ sample.variables["b"] ];
    double &c = aorg[ sample.variables["c"] ];

    a = 1;
    b = 1;
    c = 0;
    Fit::LS<double> fit;
    fit.verbose = true;
    used[1] = true;
    used[2] = true;
    used[3] = true;
    if( fit.run(sample,F, aorg, used, aerr) )
    {
        sample.display(std::cerr, aorg, aerr, "\t" );
    }

    {
        ios::wcstream fp("fit0.dat");
        for(size_t i=1;i<=N;++i)
        {
            fp("%g %g %g\n", X[i], Y[i], Z[i]);
        }
    }






}
YOCTO_UNIT_TEST_DONE()

