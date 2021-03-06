#include "yocto/utest/run.hpp"
#include "yocto/math/fit/glsf-spec.hpp"
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
        double compute( double t, const array<double> &a )
        {
            ++ncall;
            const double t0    = a[1];
            const double slope = a[2];
            return sqrt( slope*(t-t0) );
        }

    };
}



YOCTO_UNIT_TEST_IMPL(glsf)
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

    GLS<double>::Samples samples(2);
    GLS<double>::Sample &S1 = samples.append(t1, x1, z1);
    GLS<double>::Sample &S2 = samples.append(t2, x2, z2);

    samples.prepare(3,2);

    vector<double> aorg(3);
    vector<bool>   used(aorg.size(),true);
    vector<double> aerr(aorg.size());
    double &slope1  = aorg[1];
    double &slope2  = aorg[2];
    double &t0      = aorg[3];

    S1.link(1,3);
    S1.link(2,1);
    std::cerr << "S1.Gamma=" << S1.Gamma << std::endl;

    S2.link(1,3);
    S2.link(2,2);
    std::cerr << "S2.Gamma=" << S2.Gamma << std::endl;


    diffusion diff;
    diff.ncall = 0;
    GLS<double>::Function F( &diff, & diffusion::compute );


    t0     = -100;
    slope1 = 0.02;
    slope2 = 0.01;

    //samples.computeD2(F,aorg,used);

    used[3] = true;


    if( samples.fit_with(F,aorg,used,aerr) )
    {
        std::cerr << "ncall_partial=" << diff.ncall << std::endl;
        GLS<double>::display(std::cerr, aorg, aerr);
        used[3] = true;

        if(samples.fit_with(F,aorg,used,aerr))
        {
            std::cerr << "ncall_full=" << diff.ncall << std::endl;
            GLS<double>::display(std::cerr, aorg, aerr);
            save("f1.dat",t1,z1);
            save("f2.dat",t2,z2);
        }
        else
        {
            std::cerr << "Full fit failure..." << std::endl;
        }
    }
}
YOCTO_UNIT_TEST_DONE()


YOCTO_UNIT_TEST_IMPL(glsf_poly)
{
    GLS<float>::Function poly = _GLS::Create<float,_GLS::Polynomial>();


    const size_t  N     = 20;
    const float   omega = 1.0f + 0.2f*alea.to<float>();
    vector<float> X(N);
    vector<float> Y(N);

    const size_t  dmax=4;
    matrix<float> Q(dmax+1,N);
    for(size_t i=1;i<=N;++i)
    {
        const float x = (3.14f*(i-1))/N;
        X[i] = x;
        Y[i] = sin(omega*x) + 0.05f*(0.5f-alea.to<float>());
    }


    GLS<float>::Samples samples(1);


    for(size_t d=0;d<=dmax;++d)
    {
        const size_t m = d+1;
        samples.free();
        const GLS<float>::Sample &S = samples.append(X,Y,Q[m]);
        vector<float> aorg(m);
        vector<float> aerr(m);
        vector<bool>  used(m,true);

        samples.prepare(m);
        
        _GLS::Polynomial<float>::Start(S,aorg);


        std::cerr
        << "predicted=" << aorg << std::endl;

        if(samples.fit_with(poly, aorg, used, aerr))
        {
            GLS<float>::display(std::cerr,aorg, aerr);
        }
        else
        {
            //std::cerr << "Polynomial Fit Failed" << std::endl;
        }

    }

    {
        ios::wcstream fp("poly.dat");
        for(size_t i=1;i<=N;++i)
        {
            fp("%g %g", X[i], Y[i]);
            for(size_t j=1;j<=Q.rows;++j)
            {
                fp(" %g", Q[j][i]);
            }
            fp("\n");
        }
    }

    const size_t pade_max=4;
    for(size_t m=1;m<=pade_max;++m)
    {
        samples.free();
        const GLS<float>::Sample &S = samples.append(X,Y,Q[m]); (void)S;
        vector<float> aorg(m);
        vector<float> aerr(m);
        vector<bool>  used(m,true);
        const size_t q = m/2;
        const size_t p = m-q;
        GLS<float>::Function pade = _GLS::Create<float,_GLS::Pade>(p,q);

        samples.prepare(m);
        if(samples.fit_with(pade,aorg, used, aerr))
        {
            std::cerr << "Pade Fit m=" << m << std::endl;
            GLS<float>::display(std::cerr,aorg, aerr);
        }
        else
        {
            std::cerr << "Pade Fit Failure m=" << m << std::endl;
        }
    }

    {
        ios::wcstream fp("pade.dat");
        for(size_t i=1;i<=N;++i)
        {
            fp("%g %g", X[i], Y[i]);
            for(size_t j=1;j<=pade_max;++j)
            {
                fp(" %g", Q[j][i]);
            }
            fp("\n");
        }
    }

        {
            vector<double> coef(4);
            vector<double> drvs(4);
            for(size_t i=1;i<=coef.size();++i) coef[i] = i;
            
            std::cerr << "coef=" << coef << std::endl;
            _GLS::Polynomial<double>::ComputeDrvs(drvs,coef);
            std::cerr << "drvs=" << drvs << std::endl;

        }


}
YOCTO_UNIT_TEST_DONE()

#include "yocto/math/alg/spike.hpp"

static inline double make_gauss( const double t, const double a, const double mu, const double sig)
{
    const double arg = (t-mu)/sig;
    return a* exp( -0.5 * arg*arg );
}

#include "yocto/sort/quick.hpp"

YOCTO_UNIT_TEST_IMPL(glsf_gauss)
{
    GLS<double>::Function gauss = _GLS::Create<double,_GLS::Gauss>();

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


    vector<double> aorg(6);
    vector<bool>   used(6,true);
    vector<double> aerr(6);

    aorg[1] = a1;
    aorg[2] = 1;
    aorg[3] = 20;

    aorg[4] = a2;
    aorg[5] = 1;
    aorg[6] = 70;

    GLS<double>::Samples samples(1);

    samples.append(X,Y,Z);
    samples.prepare( aorg.size() );


    {
        ios::wcstream fp("gauss.dat");
        for(size_t i=1;i<=N;++i)
        {
            fp("%g %g %g\n", X[i], Y[i], Z[i]);
        }
    }

    if(samples.fit_with(gauss,aorg,used,aerr) )
    {
        GLS<double>::display(std::cerr, aorg, aerr);
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

