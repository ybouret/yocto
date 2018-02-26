
#ifndef YOCTO_MATH_FIT_POLY_INCLUDED
#define YOCTO_MATH_FIT_POLY_INCLUDED 1

#include "yocto/math/fit/fit.hpp"

namespace yocto
{
    namespace math
    {

        namespace Fit
        {

            template <typename T>
            class Poly
            {
            public:
                inline explicit Poly() throw() {}
                inline virtual ~Poly() throw() {}

                static inline
                void Create(Variables &v, const size_t degree, const string &pfx )
                {
                    v.free();
                    v.ensure(degree+1);
                    for(size_t d=0;d<=degree;++d)
                    {
                        const string id = pfx + vformat("%u",unsigned(d));
                        v(id);
                    }
                }

                inline T compute( const T x, const array<T> &a, const Variables &v) throw()
                {
                    assert(a.size()==v.size());
                    return Eval(x,a);
                }

                inline T Eval(const T X, const array<T> &aorg) throw()
                {
                    const size_t n = aorg.size();
                    if(n>0)
                    {
                        T ans = aorg[n];
                        for(size_t i=n;i>1;)
                        {
                            ans *= X;
                            ans += aorg[--i];
                        }
                        return ans;
                    }
                    else
                    {
                        return 0;
                    }
                }


            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Poly);
            };

        }

    }

}

#endif

