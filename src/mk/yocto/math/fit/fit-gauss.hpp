
#ifndef YOCTO_MATH_FIT_GAUSS_INCLUDED
#define YOCTO_MATH_FIT_GAUSS_INCLUDED 1

#include "yocto/math/fit/fit.hpp"

namespace yocto
{
    namespace math
    {
        
        namespace Fit
        {

            template <typename T>
            class Gauss
            {
            public:
                size_t calls;
                inline explicit Gauss() throw() : calls(0) {}
                inline virtual ~Gauss() throw() {}

                static inline
                void Create(Variables &v, const size_t ng, const string &pfx )
                {
                    v.free();
                    v.ensure(ng*3);
                    for(size_t g=1;g<=ng;++g)
                    {
                        const string sfx = vformat("%u",unsigned(g));
                        const string coef = pfx + "coef" + sfx; v(coef);
                        const string mu   = pfx + "mu"   + sfx; v(mu);
                        const string sig  = pfx + "sig"  + sfx; v(sig);
                    }
                }



                inline T compute( const T x, const array<T> &a, const Variables &v) throw()
                {
                    assert(a.size()==v.size());
                    ++calls;
                    const size_t nvar = a.size();
                    assert(0==(nvar%3));
                    const size_t ng   = a.size()/3;
                    size_t j=0;
                    T ans(0);
                    for(size_t i=1;i<=ng;++i)
                    {
                        const T coef = a[++j];
                        const T mu   = a[++j];
                        const T sig  = a[++j];
                        const T dx   = x-mu;
                        const T sig2 = sig*sig;
                        ans += coef * Exp( - (dx*dx)/(sig2+sig2) );
                    }
                    return ans;
                }


            };

        }

        
    }
}

#endif

