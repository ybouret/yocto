#include "yocto/alchemy/equilibria.hpp"
#include "yocto/code/utils.hpp"
#include "yocto/math/core/tao.hpp"

namespace yocto
{
    using namespace math;

    namespace alchemy
    {

        void equilibria:: mix(array<double>       &CC,
                              const double         weightA,
                              const array<double> &CA,
                              const double         weightB,
                              const array<double> &CB,
                              const double         t)
        {
            assert(weightA>=0);
            assert(weightB>=0);
            assert(weightA+weightB>0);
            assert(CA.size()>=M);
            assert(CB.size()>=M);
            assert(CC.size()>=M);
            for(size_t i=M;i>0;--i)
            {
                CC[i] = weightA*CA[i] + weightB*CB[i];
            }
            normalize(CC,t);
        }

    }

}

