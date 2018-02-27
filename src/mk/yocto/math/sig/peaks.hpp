
#ifndef YOCTO_MATH_SIG_PEAKS_INCLUDED
#define YOCTO_MATH_SIG_PEAKS_INCLUDED 1

#include "yocto/sequence/vector.hpp"
#include "yocto/sort/ysort.hpp"
#include "yocto/math/types.hpp"

#include "yocto/ios/ocstream.hpp"
#include "yocto/sequence/scroll.hpp"
#include "yocto/math/stat/descr.hpp"
#include "yocto/code/utils.hpp"

namespace yocto
{
    namespace math
    {

        struct peaks
        {
            template <typename T>
            static inline
            void build_signal(const array<T> &X,
                              const array<T> &Y,
                              const size_t    w=2)
            {
                const size_t N = Y.size();
                vector<T>      data(N);
                vector<size_t> indx(N);
                vector<T> ave(N);
                vector<T> sig(N);

                for(size_t i=N;i>0;--i)
                {
                    indx[i] = i;
                    data[i] = Y[i];
                }
                yCoSort(data,indx,__compare<T>);

                for(size_t i=1+w;i<=N-w;++i)
                {
                    T sum_x  = 0;
                    T sum_x2 = 0;
                    size_t n = 0;
                    for(size_t j=i-w;j<=i+w;++j)
                    {
                        sum_x  += data[j];
                        sum_x2 += Square(data[j]);
                        ++n;
                    }
                    ave[i] = sum_x/(n);
                    const T var = (sum_x2 - (sum_x * sum_x)/n) / (n-1);
                    sig[i] = Sqrt(var);
                }


                for(size_t i=1;i<1+w;++i)
                {
                    ave[i] = ave[1+w];
                    sig[i] = sig[1+w];
                }
                for(size_t i=N-w+1;i<=N;++i)
                {
                    ave[i] = ave[N-w];
                    sig[i] = sig[N-w];
                }

                {
                    ios::wcstream fp("signal.dat");
                    for(size_t i=1+w;i<=N-w;++i)
                    {
                        fp("%g %g %g %g %g\n", X[i], data[i], ave[i], sig[i], (X[i]-ave[i])/sig[i]);
                    }
                }
            }

        };

    }
}

#endif

