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
            assert(impl);
            delete static_cast<MATRIX *>(impl);
        }



#define MACHINE_PRECISION numeric<real_t>::epsilon

        template <>
        derivatives<real_t>:: derivatives() :
        impl( new MATRIX(NTAB,NTAB) ),
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
        real_t derivatives<real_t>:: compute( numeric<real_t>::function &f, const real_t x, const real_t h, real_t &err)
        {
            MATRIX &a = *static_cast<MATRIX *>(impl);

            real_t hh  = Fabs(h);
            real_t ans = (a[1][1]=EVAL());
            err=BIG;
            for(size_t i=2;i<=NTAB;i++)
            {
                hh /= CON;
                a[1][i]=EVAL();
                real_t       fac=CON2;
                const size_t im = i-1;
                for (size_t j=2;j<=i;j++)
                {
                    const size_t jm = j-1;
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


        
    }
}
