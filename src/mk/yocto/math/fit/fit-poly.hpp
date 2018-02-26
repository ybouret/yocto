
#ifndef YOCTO_MATH_FIT_POLY_INCLUDED
#define YOCTO_MATH_FIT_POLY_INCLUDED 1

#include "yocto/math/fit/fit.hpp"
#include "yocto/math/core/lu.hpp"
#include "yocto/code/ipower.hpp"

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

                static inline bool Start(array<T>       &a,
                                         const array<T> &X,
                                         const array<T> &Y)
                {
                    const size_t nvar = a.size();
                    assert(X.size()==Y.size());
                    if(nvar<=0) return false;
                    const size_t N      = X.size();
                    matrix<T>    mat(nvar);
                    for(size_t k=nvar;k>0;--k)
                    {
                        T                        &rhs = a[k];
                        typename matrix<T>::row  &m_k = mat[k];
                        for(size_t i=N;i>0;--i)
                        {
                            const T Xi  = X[i];
                            const T Xik = ipower<T>(Xi,k-1);
                            rhs  += Xik * Y[i];
                            for(size_t l=k;l>0;--l)
                            {
                                m_k[l] += Xik * ipower<T>(Xi,l-1);
                            }
                        }
                    }
                    for(size_t k=nvar;k>0;--k)
                    {
                        for(size_t l=k-1;l>0;--l)
                        {
                            mat[l][k] = mat[k][l];
                        }
                    }
                    if(!LU<T>::build(mat)) return false;
                    LU<T>::solve(mat,a);
                    return true;
                }

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Poly);
            };

        }

    }

}

#endif

