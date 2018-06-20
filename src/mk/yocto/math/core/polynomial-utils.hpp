#ifndef YOCTO_MATH_POLYNOMIAL_UTILS_INCLUDED
#define YOCTO_MATH_POLYNOMIAL_UTILS_INCLUDED 1

#include "yocto/math/core/polynomial.hpp"

namespace yocto
{
    namespace math
    {
        struct __poly
        {
            //! compute the continuous polynomial
            /**
             P(t)   = f1 + df1 * (t-t1) + (t-t1)^2 * A + (t-t2)^3 * B
             P(t2)  = f2
             P'(t2) = df2
             */
            template <typename T>
            void gap(polynomial<T> &P,
                     const T t1,
                     const T f1,
                     const T df1,
                     const T t2,
                     const T f2,
                     const T df2)
            {
                P.clear();
                P.add(0,f1);
                P.add(1,df1);
                const T delta  = t2-t1;
                const T u      = (f2-f1)-delta*df1;
                const T v      = (df2-df1);
                const T delta2 = delta*delta;
                const T delta3 = delta2*delta;
                const T A      = (delta+delta+delta) * u - delta2 * v;
                const T B      = delta*v-(u+u);
                P.add(2,A/delta3);
                P.add(3,B/delta3);
            }
        };
    }
}

#endif

