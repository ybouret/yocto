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

#define INI_T0 -100
#define INI_S1 0.02
#define INI_S2 0.01

    t0     = INI_T0;
    slope1 = INI_S1;
    slope2 = INI_S2;

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

    // fitting
    std::cerr << std::endl;
    std::cerr << "full fit, standard" << std::endl;

    Fit::LS<double> lsf;
    vector<bool>    used(nvar,true);
    vector<double>  aerr(nvar);
    lsf.verbose = 1;

    diff.ncall = 0;
    if( !lsf.run(samples, F, aorg, used, aerr) )
    {
        throw exception("couldn't fit, standard");
    }
    std::cerr << "ncall=" << diff.ncall << std::endl;
    samples.display(std::cerr, aorg, aerr, "\t");
    save("f1.dat",t1,z1);
    save("f2.dat",t2,z2);

    std::cerr << "Rsq         =" << lsf.Rsq << std::endl;
    std::cerr << "sample1.D2  =" << sample1.D2 << std::endl;
    std::cerr << "sample2.D2  =" << sample2.D2 << std::endl;
    std::cerr << "sample1.corr=" << sample1.correlation() << std::endl;
    std::cerr << "sample2.corr=" << sample2.correlation() << std::endl;

    diff.ncall  = 0;
    samples.computeD2(F,aorg);
    std::cerr << "calls/D2=" << diff.ncall << std::endl;
    
    if(true)
    {
        std::cerr << std::endl;
        std::cerr << "partial fit" << std::endl;
        t0     = INI_T0;
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
    lsf.verbose = true;


    
    std::cerr << std::endl;
    Fit::Poly<double>::Start(aorg,X,Y);
    poly.calls   = 0;
    std::cerr << "start=" << aorg << std::endl;
    if( !lsf.run(sample, F, aorg, used, aerr) )
    {
        throw exception("couldn't fit poly");
    }
    std::cerr << "#calls=" << poly.calls << std::endl;
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


#include "yocto/math/fit/fit-gauss.hpp"
#include "yocto/math/alg/spike.hpp"
#include "yocto/sort/quick.hpp"
    
    static inline double make_gauss( const double t, const double a, const double mu, const double sig)
    {
        const double arg = (t-mu)/sig;
        return a* exp( -0.5 * arg*arg );
    }

    YOCTO_UNIT_TEST_IMPL(fit_gauss)
    {
        Fit::Gauss<double>           gauss;
        Fit::Type<double>::Function F( &gauss, & Fit::Gauss<double>::compute );
        
        const size_t  N     = 200;
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
            Y[i] = make_gauss(t,a1, mu1, sig1) + make_gauss(t, a2, mu2, sig2) + 1*(0.5-alea.to<double>());
        }
        
        {
            ios::wcstream fp("gdata.dat");
            for(size_t i=1;i<=N;++i)
            {
                fp("%g %g\n", X[i], Y[i]);
            }
        }
        
        
        std::cerr << "dectecting spikes" << std::endl;
        vector<spike::pointer> spikes;
        spike::detect(spikes,Y);
        std::cerr << "#spikes=" << spikes.size() << std::endl;
        quicksort(spikes,spike::compare_by_value);
        
        if(spikes.size()<2)
            throw exception("Not enough spikes!");
        
        const spike &s1 = *spikes[1];
        const spike &s2 = *spikes[2];
        
        {
            ios::wcstream fp("spike1.dat");
            for(size_t i=s1.lower;i<=s1.upper;++i)
            {
                fp("%g %g\n", X[i], Y[i]);
            }
        }
        
        {
            ios::wcstream fp("spike2.dat");
            for(size_t i=s2.lower;i<=s2.upper;++i)
            {
                fp("%g %g\n", X[i], Y[i]);
            }
        }
        
        Fit::Sample<double> sample(X,Y,Z);
        Fit::Gauss<double>::Create(sample.variables,2,"");
        const size_t   nvar = sample.variables.size();
        vector<double> aorg(nvar);
        vector<bool>   used(nvar,true);
        vector<double> aerr(nvar);
        
        aorg[1] = a1;
        aorg[2] = 20;
        aorg[3] = 1;
        
        aorg[4] = a2;
        aorg[5] = 70;
        aorg[6] = 1;
        
        Fit::LS<double> lsf;
        //lsf.verbose = true;
        if( lsf.run(sample, F, aorg, used, aerr) )
        {
            sample.display(std::cerr, aorg, aerr);
        }
        else
        {
            std::cerr << "error" << std::endl;
        }
        
        {
            ios::wcstream fp("gauss.dat");
            for(size_t i=1;i<=N;++i)
            {
                fp("%g %g %g\n", X[i], Y[i], Z[i]);
            }
        }
        
    }
    YOCTO_UNIT_TEST_DONE()
