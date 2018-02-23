#include "yocto/math/fcn/derivatives.hpp"
#include "yocto/math/ztype.hpp"
#include "yocto/container/matrix.hpp"
#include "yocto/math/types.hxx"
#include "yocto/math/round.hpp"
#include "yocto/exceptions.hpp"
#include "yocto/code/utils.hpp"
#include <cerrno>

namespace yocto
{
    namespace math
    {
#define MATRIX matrix<real_t>
#define CON  REAL(1.4)
#define CON2 (CON*CON)
#define NTAB 10
#define SAFE REAL(2.0)
#define BIG  (REAL_MAX/SAFE)

        template <>
        derivatives<real_t>:: ~derivatives() throw()
        {

        }

        template <>
        real_t derivatives<real_t>:: eval_field(real_t X)
        {
            assert(call);
            assert(aptr);

            scalar_field        & f = *static_cast<scalar_field *>(call);
            const array<real_t> &_x = *aptr;
            array<real_t>       & x = (array<real_t> &)_x;
            assert(ivar>0); assert(ivar<=x.size());

            const real_t xsav = X;
            x[ivar] = X;
            const real_t ans = f(x);
            x[ivar] = xsav;
            return ans;

        }

        template <>
        real_t derivatives<real_t>:: eval_pfunc(real_t value)
        {
            assert(call);
            assert(aptr);

            parametric_function & f = *static_cast<parametric_function*>(call);
            const array<real_t> & x = *aptr;
            
            return f(value,x);
        }

        template <>
        real_t derivatives<real_t>:: eval_fitfn(real_t X)
        {
            assert(call);
            assert(aptr);

            parametric_function & f = *static_cast<parametric_function*>(call);
            const array<real_t> &_x = *aptr;
            array<real_t>       & x = (array<real_t> &)_x;
            assert(ivar>0); assert(ivar<=x.size());

            const real_t xsav = X;
            x[ivar] = X;
            const real_t ans = f(vpar,x);
            x[ivar] = xsav;
            return ans;

        }

#define MACHINE_PRECISION numeric<real_t>::epsilon

        template <>
        derivatives<real_t>:: derivatives() :
        a(NTAB,NTAB),
        ivar(0),
        call(0),
        aptr(0),
        fdir(this, & derivatives<real_t>::eval_field ),
        fpar(this, & derivatives<real_t>::eval_pfunc ),
        ffit(this, & derivatives<real_t>::eval_fitfn ),
        vpar(0),
        max_ftol( log_round_ceil( Sqrt(MACHINE_PRECISION) ) ),
        opt_step( log_round_ceil( Pow(MACHINE_PRECISION,REAL(1.0)/REAL(3.0)) ) )
        {

        }

        template <>
        bool derivatives<real_t>:: regularize(const real_t x,real_t &h) throw()
        {
            volatile real_t temp = x+h;
            h=temp-x;
            return (x+h>x);
        }

        static inline real_t __eval( numeric<real_t>::function &f, const real_t x, real_t h)
        {
            if(! derivatives<real_t>::regularize(x,h) )
            {
                throw libc::exception( EDOM, "underflow in derivatives" );
            }
            return ( f(x+h)-f(x-h) ) / (h+h);
        }

#define EVAL() __eval(f,x,hh)

        template <>
        real_t derivatives<real_t>:: diff(function     &f,
                                          const real_t  x,
                                          const real_t  h,
                                          real_t       &err)
        {
            
            real_t hh  = Fabs(h);
            real_t ans = (a[1][1]=EVAL());
            err=BIG;
            for(size_t i=2,im=i-1;i<=NTAB;++i,++im)
            {
                hh /= CON;
                a[1][i]=EVAL();
                real_t fac=CON2;
                for (size_t j=2,jm=j-1;j<=i;++j,++jm)
                {
                    a[j][i]=(a[jm][i]*fac-a[jm][im])/(fac-REAL(1.0));
                    fac=CON2*fac;
                    const real_t errt=max_of(Fabs(a[j][i]-a[jm][i]),Fabs(a[j][i]-a[jm][im]));
                    if (errt <= err)
                    {
                        err=errt;
                        ans=a[j][i];
                    }
                }

                if (Fabs(a[i][i]-a[im][im]) >= SAFE*err)
                {
                    break;
                }
            }

            return ans;
        }

        template <>
        real_t derivatives<real_t>:: compute(function &f, const real_t x, real_t h)
        {
            // initialize
            real_t err  = 0;
            real_t dFdx = diff(f, x, h, err);
            while( err > max_ftol * Fabs(dFdx) )
            {
                real_t new_err = 0;
                h /= CON;
                const real_t new_dFdx = diff(f,x,h,new_err);
                if(new_err>=err)
                    break;
                err  = new_err;
                dFdx = new_dFdx;
            }

            return dFdx;
        }

        template <>
        real_t  derivatives<real_t>::compute(parametric_function &f,
                                             const real_t         x,
                                             const array<real_t> &param,
                                             const real_t         h )
        {
            call = &f;
            aptr = &param;
            return compute(fpar,x,h);
        }



        template <>
        void derivatives<real_t>::gradient(array<real_t>      &grad,
                                           scalar_field       &f,
                                           const array<real_t> &x,
                                           const array<real_t> &h )
        {
            const size_t nvar = x.size(); assert(h.size()==x.size()); assert(grad.size()==x.size());
            call = & f;
            aptr = & x;
            for(ivar=nvar;ivar>0;--ivar)
            {
                grad[ivar] = compute( fdir, x[ivar], h[ivar] );
            }
        }

        template <>
        void derivatives<real_t>:: gradient(array<real_t>       &grad,
                                            parametric_function &f,
                                            const real_t         u,
                                            const array<real_t> &x,
                                            const array<real_t> &h )
        {
            const size_t nvar = x.size(); assert(h.size()==x.size()); assert(grad.size()==x.size());
            call = & f;
            aptr = & x;
            vpar = u;
            for(ivar=nvar;ivar>0;--ivar)
            {
                grad[ivar] = compute( ffit, x[ivar], h[ivar] );
            }
        }


        
    }
}
