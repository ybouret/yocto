
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

                inline T Eval(const T x, const T mu, const T sigma)
                {
                    const T sig2 = sigma * sigma;
                }

                inline T compute( const T x, const array<T> &a, const Variables &v) throw()
                {
                    assert(a.size()==v.size());
                    ++calls;
                    return Eval(x,a);
                }


            };

        }

        
    }
}

#endif

