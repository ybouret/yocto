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
        double compute( double t, const array<double> &a )
        {
            ++ncall;
            const double t0    = a[1];
            const double slope = a[2];
            return sqrt( slope*(t-t0) );
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

    Fit<double>::Samples samples(2);
    Fit<double>::Sample &sample1 = samples.add(t1,x1,z1);
    Fit<double>::Sample &sample2 = samples.add(t2,x2,z2);

    FitVariables &global = samples.variables;
    global << "t0" << "slope1" << "slope2";
    sample1.variables("t0")("slope","slope1");
    sample2.variables("t0")("slope","slope2");

    std::cerr << "global =" << global << std::endl;
    std::cerr << "sample1=" << sample1.variables << std::endl;
    std::cerr << "sample2=" << sample2.variables << std::endl;




}
YOCTO_UNIT_TEST_DONE()


