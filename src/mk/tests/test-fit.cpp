#include "yocto/math/fit/fit.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/sequence/lw-array.hpp"
#include "yocto/ios/ocstream.hpp"

using namespace yocto;
using namespace math;

static inline void save(const string &fn, const array<double> &x, const array<double> &y)
{
    ios::ocstream fp(fn,false);
    for(size_t i=1;i<=x.size();++i)
    {
        fp("%g %g\n", x[i], y[i]);
    }
}

namespace
{
    struct diffusion
    {

        size_t ncall;
        double compute( double t, const array<double> &a, const Fit::Variables &var)
        {
            ++ncall;
            const double t0    = a[ var["t0"]    ];
            const double slope = a[ var["slope"] ];
            return sqrt( slope*Fabs((t-t0)) );
        }

    };
}


YOCTO_UNIT_TEST_IMPL(fit)
{
    const double _t1[] = { 30,80,140,200,270,320,430,550,640,720,830,890 };
    const double _x1[] = { 4.414520,5.011710,5.632319,6.194379,6.721311,7.330211,8.009368,8.735363,9.297424,9.707260,10.339578,10.878220};

    const double _t2[] = { 60,120,200,270,360,460,580,670,790,920,1040 };
    const double _x2[] = { 5.199063,5.854801,6.662763,7.365340,8.067916,8.782201,9.578454,10.175644,10.878220,11.651054,12.213115};

    const lw_array<double> t1( (double*)_t1, sizeof(_t1)/sizeof(_t1[0]) );
    const lw_array<double> x1( (double*)_x1, sizeof(_x1)/sizeof(_x1[0]) );
    const lw_array<double> t2( (double*)_t2, sizeof(_t2)/sizeof(_t2[0]) );
    const lw_array<double> x2( (double*)_x2, sizeof(_x2)/sizeof(_x2[0]) );

    save("d1.dat",t1,x1);
    save("d2.dat",t2,x2);


    vector<double> z1(t1.size(),0);
    vector<double> z2(t2.size(),0);

    Fit::Samples<double> samples(2,3);
    Fit::Sample<double> &sample1 = samples.add(t1,x1,z1,2);
    Fit::Sample<double> &sample2 = samples.add(t2,x2,z2,2);



    samples.variables << "slope1" << "slope2" << "t0";
    sample1.variables("slope","slope1")("t0");
    sample2.variables("slope","slope2")("t0");

    samples.link();

    std::cerr << "samples=" << samples.variables << std::endl;
    std::cerr << "       |_indices=" << samples.indices << std::endl;
    std::cerr << "sample1=" << sample1.variables << std::endl;
    std::cerr << "       |_indices=" << sample1.indices << std::endl;
    std::cerr << "sample2=" << sample2.variables << std::endl;
    std::cerr << "       |_indices=" << sample2.indices << std::endl;
    std::cerr << std::endl;;

    const Fit::Variables &var = samples.variables;
    const size_t nvar = var.size();
    vector<double> aorg(nvar);
    double &t0     = aorg[ var["t0"] ];
    double &slope1 = aorg[ var["slope1"] ];
    double &slope2 = aorg[ var["slope2"] ];

    t0     = -100;
    slope1 = 0.02;
    slope2 = 0.01;

    diffusion diff = { 0 };
    Fit::Type<double>::Function F( &diff, & diffusion::compute );

    const double Ds = samples.computeD2(F,aorg);
    const double D1 = sample1.computeD2(F,aorg);
    const double D2 = sample2.computeD2(F,aorg);
    std::cerr << "Ds=" << Ds << std::endl;
    std::cerr << "D1=" << D1 << std::endl;
    std::cerr << "D2=" << D2 << std::endl;
    std::cerr << std::endl;;

    Fit::Gradient<double> grad;
    const double D1b = sample1.computeD2(F,aorg,grad);
    std::cerr << "D1b  =" << D1b << std::endl;
    std::cerr << "beta1=" << sample1.beta  << std::endl;
    std::cerr << "curv1=" << sample1.curv  << std::endl;

    const double D2b = sample2.computeD2(F,aorg,grad);
    std::cerr << "D2b  =" << D2b << std::endl;
    std::cerr << "beta2=" << sample2.beta  << std::endl;
    std::cerr << "curv2=" << sample2.curv  << std::endl;

    const double Dsb = samples.computeD2(F,aorg,grad);
    std::cerr << "Dsb  =" << Dsb << std::endl;
    std::cerr << "betaS=" << samples.beta  << std::endl;
    std::cerr << "curvS=" << samples.curv  << std::endl;

    std::cerr << std::endl;
    std::cerr << "full fit..." << std::endl;
    Fit::LS<double> lsf;
    vector<bool>    used(nvar,true);
    vector<double>  aerr(nvar);
    if( !lsf.run(samples, F, aorg, used, aerr) )
    {
        throw exception("couldn't fit");
    }
    std::cerr << "ncall=" << diff.ncall << std::endl;
    std::cerr << "aorg =" << aorg << std::endl;
    std::cerr << "aerr =" << aerr << std::endl;
    save("f1.dat",t1,z1);
    save("f2.dat",t2,z2);

    std::cerr << "Rsq=" << lsf.Rsq << std::endl;

    samples.display(std::cerr, aorg, aerr, "\t");
    std::cerr << "sample1.D2  =" << sample1.D2 << std::endl;
    std::cerr << "sample2.D2  =" << sample2.D2 << std::endl;
    std::cerr << "sample1.corr=" << sample1.correlation() << std::endl;
    std::cerr << "sample2.corr=" << sample2.correlation() << std::endl;

    std::cerr << std::endl;
    std::cerr << "partial fit" << std::endl;
    t0     = -100;
    slope1 *= 1 + 0.1 * alea.symm<double>();
    slope2 *= 1 + 0.1 * alea.symm<double>();
    used[var["slope1"]] = false;
    used[var["slope2"]] = false;
    if( !lsf.run(samples, F, aorg, used, aerr) )
    {
        throw exception("couldn't fit");
    }
    samples.display(std::cerr, aorg, aerr, "\t");


}
YOCTO_UNIT_TEST_DONE()

#include "yocto/math/fit/fit-poly.hpp"
#include "yocto/sort/ysort.hpp"
#include "yocto/string/conv.hpp"

YOCTO_UNIT_TEST_IMPL(fit_poly)
{
    size_t degree = 2;
    if(argc>1)
    {
        degree = strconv::to<size_t>(argv[1],"degree");
    }
    Fit::Poly<double>           poly;
    Fit::Type<double>::Function F( &poly, & Fit::Poly<double>::compute );

    const size_t   N = 10 + alea.leq(50);
    vector<double> X(N);
    vector<double> Y(N);
    vector<double> Z(N);
    for(size_t i=1;i<=N;++i)
    {
        X[i] = 1.6 * alea.get<double>();
        Y[i] = sin(X[i]) + 0.1 * (alea.get<double>()-0.5);
    }
    yCoSort(X,Y,__compare<double>);

    Fit::Sample<double> sample(X,Y,Z);
    Fit::Poly<double>::Create(sample.variables,degree, "a");
    std::cerr << "var=" << sample.variables << std::endl;

    const size_t    nvar = sample.variables.size();
    vector<double>  aorg(nvar);
    vector<double>  aerr(nvar);
    vector<bool>    used(nvar,true);
    Fit::LS<double> lsf;

    if( !lsf.run(sample, F, aorg, used, aerr) )
    {
        throw exception("couldn't fit poly");
    }
    sample.display(std::cerr, aorg, aerr, "\t");

    {
        ios::wcstream fp("poly.dat");
        for(size_t i=1;i<=N;++i)
        {
            fp("%g %g %g\n", X[i], Y[i], Z[i] );
        }
    }

}
YOCTO_UNIT_TEST_DONE()

