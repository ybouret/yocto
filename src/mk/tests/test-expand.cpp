#include "yocto/math/sig/smooth.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/string/conv.hpp"
#include "yocto/math/core/tao.hpp"

using namespace yocto;
using namespace math;

struct Expand
{
    const expand *xp;
    const char   *id;
};

template <typename T>
static inline void save_xp( const Expand &ctx, const array<T> &X, const array<T> &Y )
{
    const unit_t  n   = X.size();
    const string  sfx = ".dat";
    const string  fn  = ctx.id + sfx;
    ios::wcstream fp(fn);
    const unit_t delta = n+n/2;
    for(unit_t i=-delta;i<=n+delta;++i)
    {
        fp("%g %g\n", ctx.xp->get_x(i, X), ctx.xp->get_y(i,Y) );
    }
}

YOCTO_UNIT_TEST_IMPL(expand)
{

    double       NOISE  = 0.2;
    double       dt     = 0.4;
    size_t       degree = 2;

    if(argc>1)
    {
        dt = strconv::to<double>(argv[1],"dt");
    }

    if(argc>2)
    {
        degree = strconv::to<size_t>(argv[2],"degree");
    }


    const unit_t    n=50+alea.leq(100);
    vector<double>  x(n,0.0);
    vector<double>  y(n,0.0);
    vector<double>  z(n,0.0);
    for( size_t i=2; i <= n; ++i )
    {
        x[i] = x[i-1] + 0.5 + alea.to<double>();
    }

    const double fac   = (numeric<double>::two_pi / x[n]);

    for( size_t i=1; i <= n; ++i )
    {
        x[i] *= fac;
        y[i] = 0.2+sin(x[i]) + sin(3*x[i]);
        z[i] = y[i] + NOISE * alea.symm<double>();
    }
    y[n] = y[1];
    z[1] = y[1];
    z[n] = y[n];

    {
        ios::wcstream fp("xdata.dat");
        for(size_t i=1;i<=n;++i)
        {
            fp("%g %g %g\n", x[i], z[i], y[i]);
        }
    }

    expand xp_nul(expand::zero);
    expand xp_cst(expand::constant);
    expand xp_cyc(expand::cyclic);
    expand xp_odd(expand::odd);
    expand xp_evn(expand::even);

    const Expand xps[] =
    {
        { &xp_nul, "nul" },
        { &xp_cst, "cst" },
        { &xp_cyc, "cyc" },
        { &xp_odd, "odd" },
        { &xp_evn, "evn" }
    };
    const size_t num = sizeof(xps)/sizeof(xps[0]);

    for(size_t j=0;j<num;++j)
    {
        save_xp(xps[j],x,z);
    }


#if 0
    expand<double> xp_constant(expand_constant);
    expand<double> xp_cyclic(expand_cyclic);
    expand<double> xp_odd(expand_odd);
    expand<double> xp_even(expand_even);


    {
        ios::wcstream fp("xdata.dat");
        for(size_t i=1;i<=n;++i)
        {
            fp("%g %g %g\n", x[i], z[i], y[i]);
        }
    }

    const unit_t  nn = n;
    {
        ios::wcstream fp_cst("xpand_cst.dat");
        ios::wcstream fp_cyc("xpand_cyc.dat");
        ios::wcstream fp_odd("xpand_odd.dat");
        ios::wcstream fp_evn("xpand_evn.dat");

        for(unit_t i=-nn;i<=nn+nn;++i)
        {
            fp_cst("%g %g\n", xp_constant.get_x(i, x, nn), xp_constant.get_y(i, y, nn) );
            fp_cyc("%g %g\n", xp_cyclic.get_x(i, x, nn),   xp_cyclic.get_y(i, y, nn) );
            fp_odd("%g %g\n", xp_odd.get_x(i, x, nn),      xp_odd.get_y(i, y, nn) );
            fp_evn("%g %g\n", xp_even.get_x(i, x, nn),     xp_even.get_y(i, y, nn) );
        }
    }


    smooth<double> sm;
    sm.upper_range = sm.lower_range = dt/2;
    sm.degree      = degree;


    sm.compute(xp_cyclic,1,x,z,nn);
    {
        ios::wcstream fp("head_cyclic.dat");
        for( list< point2d<double> >::iterator i=sm.points.begin();i!=sm.points.end();++i)
        {
            const point2d<double> &p = *i;
            fp("%g %g\n", p.x, p.y);
        }
    }

    sm.compute(xp_odd,1,x,z,nn);
    {
        ios::wcstream fp("head_odd.dat");
        for( list< point2d<double> >::iterator i=sm.points.begin();i!=sm.points.end();++i)
        {
            const point2d<double> &p = *i;
            fp("%g %g\n", p.x, p.y);
        }
    }

    vector<double> ys(n);
    vector<double> dy(n);
    vector<double> dy2(n);
    sm(xp_cyclic,ys,x,z,&dy);
    sm(xp_cyclic,ys,x,z,dy2);
    {
        ios::wcstream fp("sm_cyclic.dat");
        for(size_t i=1;i<=n;++i)
        {
            fp("%g %g %g %g\n", x[i], ys[i], dy[i], dy2[i]);
        }

    }

    std::cerr << "computing RMS..." << std::endl;
    {
        ios::wcstream fp("rms.dat");
        ios::wcstream fp2("xsm.dat");
        for(size_t i=1;i<=n/2;i+=2)
        {
            dt = x[i]-x[1];
            sm.lower_range = dt/2;
            sm.upper_range = dt/2;
            sm(xp_odd,ys,x,z,NULL);
            const double rms = tao::RMS(ys,z);
            fp("%g %g\n", dt,rms);
            for(size_t j=1;j<=n;++j)
            {
                fp2("%g %g %g\n", x[j], ys[j], double(i) );
            }
            fp2("\n");
        }
    }
    
#endif
    
}
YOCTO_UNIT_TEST_DONE()
