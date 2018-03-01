
#include "yocto/math/sig/peaks.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;

static inline double make_gauss( const double t, const double a, const double mu, const double sig)
{
    const double arg = (t-mu)/sig;
    return a* exp( -0.5 * arg*arg );
}


YOCTO_UNIT_TEST_IMPL(peaks)
{
    const size_t  N     = 100+alea.leq(500);
    const double  Tmx   = 100;
    const double  mu1   = 25;
    const double  mu2   = 72;
    const double  sig1  = 3;
    const double  sig2  = 2.7;
    const double  a1    = 9;
    const double  a2    = 3.14;

    vector<double> X(N);
    vector<double> Y(N);
    vector<double> Z(N);

    for(size_t i=1;i<=N;++i)
    {
        const double t = (i*Tmx)/N;
        X[i] = t;
        Y[i] = make_gauss(t,a1, mu1, sig1) + make_gauss(t, a2, mu2, sig2) + 0.5*alea.symm<double>();
    }

    {
        ios::wcstream fp("data.dat");
        for(size_t i=1;i<=N;++i)
        {
            fp("%g %g\n", X[i], Y[i]);
        }
    }
    math::peaks::build_signal(X,Y);



}
YOCTO_UNIT_TEST_DONE()


