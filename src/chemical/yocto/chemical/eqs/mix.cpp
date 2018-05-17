
#include "yocto/chemical/equilibria.hpp"
#include "yocto/math/core/tao.hpp"

namespace yocto
{
    using namespace math;

    namespace chemical
    {
        bool equilibria:: mix(array<double>       &Cmix,
                              const array<double> &C0, const double V0,
                              const array<double> &C1, const double V1,
                              const double t)
        {
            assert(V0>=0);
            assert(V1>=0);
            assert(V0+V1>0);
            const double Vtot = V0+V1;
            for(size_t j=M;j>0;--j)
            {
                Cmix[j] = (V0*C0[j]+V1*C1[j])/Vtot;
            }
            return normalize(Cmix,t);
        }

    }
}

